//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Collections.Generic;
using Windows.Networking.Sockets;
using System.Threading.Tasks;
using Windows.Devices.WiFiDirect.Services;
using Windows.Devices.Enumeration;
using Windows.Storage.Streams;
using System.Globalization;
using System.Threading;

namespace SDKTemplate
{
    namespace WiFiDirectServices
    {
        /// <summary>
        /// Wraps WiFiDirectServiceAdvertiser and handles callbacks and related state
        /// </summary>
        public class AdvertisementWrapper : IDisposable
        {
            private Object thisLock = new Object();

            private WiFiDirectServiceAdvertiser advertiser = null;
            // Used to update main state
            private WiFiDirectServiceManager manager = null;

            // PIN to use for display config method (if selected)
            private string pin = "";
            
            // Keep track of incoming service requests that need to be accepted or rejected
            private IList<WiFiDirectServiceSessionRequest> requestList = new List<WiFiDirectServiceSessionRequest>();


            // Keep internal identifier
            private int uniqueInternalId;
            private static int nextId = 1;

            public AdvertisementWrapper(
                WiFiDirectServiceAdvertiser advertiser,
                WiFiDirectServiceManager manager,
                string pin = ""
                )
            {
                this.advertiser = advertiser;
                this.manager = manager;
                // Store PIN for "display" config method
                this.pin = pin;

                uniqueInternalId = nextId++;

                // This should fire when the service is created and advertisement has started
                // It will also fire when the advertisement has stopped for any reason
                this.advertiser.AdvertisementStatusChanged += OnAdvertisementStatusChanged;
                // This will fire when an auto-accept session is connected. Advertiser should keep track of the new service session
                this.advertiser.AutoAcceptSessionConnected += OnAutoAcceptSessionConnected;
                // This will fire when a session is requested and it must be explicitly accepted or rejected.
                // The advertiser may need to display a PIN or take user input for a PIN
                this.advertiser.SessionRequested += OnSessionRequested;
            }

            public WiFiDirectServiceAdvertiser Advertiser
            {
                get { return advertiser; }
            }

            public int InternalId
            {
                get { return uniqueInternalId; }
            }

            public string Pin
            {
                get { return pin; }
            }

            public string Name
            {
                get { return advertiser.ServiceName; }
            }

            public string Status
            {
                get { return advertiser.AdvertisementStatus.ToString(); }
            }

            public IList<WiFiDirectServiceSessionRequest> SessionRequestList
            {
                get { return requestList; }
            }

            // NOTE: this is mutually exclusive with OnAutoAcceptSessionConnected
            private void OnSessionRequested(WiFiDirectServiceAdvertiser sender, WiFiDirectServiceSessionRequestedEventArgs args)
            {
                try
                {
                    ThrowIfDisposed();

                    manager.NotifyUser("Received session request", NotifyType.StatusMessage);
                    
                    string sessionInfo = "";
                    if (args.GetSessionRequest().SessionInfo != null && args.GetSessionRequest().SessionInfo.Length > 0)
                    {
                        using (DataReader sessionInfoDataReader = DataReader.FromBuffer(args.GetSessionRequest().SessionInfo))
                        {
                            sessionInfo = sessionInfoDataReader.ReadString(args.GetSessionRequest().SessionInfo.Length);
                            manager.NotifyUser("Received Session Info: " + sessionInfo, NotifyType.StatusMessage);
                        }
                    }
                    
                    lock (thisLock)
                    {
                        requestList.Add(args.GetSessionRequest());
                    }

                    // Now we update the UI and wait for action to take (accept or decline)
                    manager.AddSessionRequest(args.GetSessionRequest(), this);

                    if (args.GetSessionRequest().ProvisioningInfo.IsGroupFormationNeeded &&
                        args.GetSessionRequest().ProvisioningInfo.SelectedConfigurationMethod == WiFiDirectServiceConfigurationMethod.PinDisplay)
                    {
                        // This means the advertiser should display a PIN and the seeker will look at the display and enter the same PIN
                        // The PIN may be randomly generated now, but for this sample, we configure a PIN when starting the advertiser
                        manager.NotifyUser("Remote side should enter PIN: " + pin, NotifyType.StatusMessage);
                    }
                }
                catch (Exception ex)
                {
                    manager.NotifyUser("OnSessionRequest Failed: " + ex.Message, NotifyType.ErrorMessage);
                }
            }

            public async void AcceptSessionRequest(string id, string pin = "")
            {
                WiFiDirectServiceSessionRequest request = null;
                bool found = false;

                lock (thisLock)
                {
                    foreach (var r in requestList)
                    {
                        if (r.DeviceInformation.Id == id)
                        {
                            found = true;
                            request = r;
                        }
                    }

                    if (!found)
                    {
                        throw new KeyNotFoundException("Attempted to accept session not found in list");
                    }
                    
                    requestList.Remove(request);
                }
                manager.RemoveSessionRequest(request, this);

                try
                {
                    WiFiDirectServiceSession session = null;
                    string pinForConnect = pin;

                    // Determine if a PIN is needed
                    if (request.ProvisioningInfo.IsGroupFormationNeeded &&
                        request.ProvisioningInfo.SelectedConfigurationMethod == WiFiDirectServiceConfigurationMethod.PinDisplay)
                    {
                        // This means the advertiser should display a PIN and the seeker will look at the display and enter the same PIN
                        // The PIN may be randomly generated now, but for this sample, we configure a PIN when starting the advertiser
                        pinForConnect = this.pin;
                    }
                    else if (request.ProvisioningInfo.IsGroupFormationNeeded &&
                        request.ProvisioningInfo.SelectedConfigurationMethod == WiFiDirectServiceConfigurationMethod.PinEntry)
                    {
                        // This means that the seeker should display a PIN and the advertiser will need to look at that display and enter the same PIN
                        if (pin.Length == 0)
                        {
                            throw new ArgumentException("Expected PIN for connection, not provided");
                        }
                        pinForConnect = pin;
                    }
                    else
                    {
                        // PIN not needed, do not pass into connect
                        pinForConnect = "";
                    }

                    manager.NotifyUser("Accepting session request...", NotifyType.StatusMessage);

                    if (pinForConnect.Length > 0)
                    {
                        // NOTE: This MUST be called from the UI thread
                        session = await advertiser.ConnectAsync(request.DeviceInformation, pinForConnect);
                    }
                    else
                    {
                        // NOTE: This MUST be called from the UI thread
                        session = await advertiser.ConnectAsync(request.DeviceInformation);
                    }

                    manager.NotifyUser("Session request accepted", NotifyType.StatusMessage);

                    SessionWrapper sessionWrapper = new SessionWrapper(session, manager);
                    manager.AddSession(sessionWrapper);
                }
                catch (Exception ex)
                {
                    manager.NotifyUser("AcceptSessionRequest Failed: " + ex.Message, NotifyType.ErrorMessage);
                }
            }

            public void DeclineSessionRequest(string id)
            {
                WiFiDirectServiceSessionRequest request = null;

                bool found = false;

                lock (thisLock)
                {
                    foreach (var r in requestList)
                    {
                        if (r.DeviceInformation.Id == id)
                        {
                            found = true;
                            request = r;
                        }
                    }

                    if (!found)
                    {
                        throw new KeyNotFoundException("Attempted to accept session not found in list");
                    }

                    requestList.Remove(request);
                }
                manager.RemoveSessionRequest(request, this);

                // Decline incoming requests by disposing the request object
                request.Dispose();
                manager.NotifyUser("Declined Request", NotifyType.StatusMessage);
            }

            // NOTE: this is mutually exclusive with OnSessionRequested
            private void OnAutoAcceptSessionConnected(WiFiDirectServiceAdvertiser sender, WiFiDirectServiceAutoAcceptSessionConnectedEventArgs args)
            {
                try
                {
                    ThrowIfDisposed();
                    
                    string sessionInfo = "";
                    if (args.SessionInfo != null && args.SessionInfo.Length > 0)
                    {
                        using (DataReader sessionInfoDataReader = DataReader.FromBuffer(args.SessionInfo))
                        {
                            sessionInfo = sessionInfoDataReader.ReadString(args.SessionInfo.Length);
                        }
                    }

                    manager.NotifyUser("Auto-Accept Session Connected: sessionInfo=" + sessionInfo, NotifyType.StatusMessage);

                    SessionWrapper sessionWrapper = new SessionWrapper(args.Session, manager);
                    manager.AddSession(sessionWrapper);
                }
                catch (Exception ex)
                {
                    manager.NotifyUser("OnAutoAcceptSessionConnected Failed: " + ex.Message, NotifyType.ErrorMessage);
                }
            }

            private void OnAdvertisementStatusChanged(WiFiDirectServiceAdvertiser sender, object args)
            {
                try
                {
                    ThrowIfDisposed();

                    WiFiDirectServiceAdvertisementStatus status = advertiser.AdvertisementStatus;

                    manager.NotifyUser("Advertisement Status Changed to " + status.ToString(), NotifyType.StatusMessage);

                    manager.AdvertiserStatusChanged(this);
                }
                catch (Exception ex)
                {
                    manager.NotifyUser("OnAdvertisementStatusChanged Failed: " + ex.Message, NotifyType.ErrorMessage);
                }
            }

            #region Dispose
            bool disposed = false;

            ~AdvertisementWrapper()
            {
                Dispose(false);
            }

            public void Dispose()
            {
                Dispose(true);
                GC.SuppressFinalize(this);
            }

            protected virtual void Dispose(bool disposing)
            {
                if (disposed)
                {
                    return;
                }

                if (disposing)
                {
                    // Stop advertising
                    // If for some reason the advertiser is stopping between the time that we check
                    // the AdvertisementStatus and when we call Stop, this will throw
                    try
                    {
                        if (advertiser.AdvertisementStatus == WiFiDirectServiceAdvertisementStatus.Started)
                        {
                            advertiser.Stop();
                        }
                    }
                    catch (Exception)
                    {
                        // Stop can throw if it is already stopped or stopping, ignore
                    }

                    // Remove event handlers
                    advertiser.AdvertisementStatusChanged -= OnAdvertisementStatusChanged;
                    advertiser.AutoAcceptSessionConnected -= OnAutoAcceptSessionConnected;
                    advertiser.SessionRequested -= OnSessionRequested;

                    foreach (var request in requestList)
                    {
                        request.Dispose();
                    }
                    requestList.Clear();
                }

                disposed = true;
            }

            private void ThrowIfDisposed()
            {
                if (disposed)
                {
                    throw new ObjectDisposedException("AdvertisementWrapper");
                }
            }
            #endregion
        }
        
        public class DiscoveredDeviceWrapper : IDisposable
        {
            private DeviceInformation deviceInfo;
            // Used to update main state
            private WiFiDirectServiceManager manager = null;

            // When we start opening/connecting to a session we store this (only used for a single connect)
            private WiFiDirectService service = null;

            public DiscoveredDeviceWrapper(
                DeviceInformation deviceInfo,
                WiFiDirectServiceManager manager
                )
            {
                this.deviceInfo = deviceInfo;
                this.manager = manager;

                ParseProperties();
            }

            private void ParseProperties()
            {
                object serviceName;
                if (deviceInfo.Properties.TryGetValue("System.Devices.WiFiDirectServices.ServiceName", out serviceName))
                {
                    ServiceName = serviceName.ToString();
                }
                else
                {
                    ServiceName = "";
                }

                object advertisementId;
                if (deviceInfo.Properties.TryGetValue("System.Devices.WiFiDirectServices.AdvertisementId", out advertisementId))
                {
                    AdvertisementId = Convert.ToUInt32(advertisementId, CultureInfo.InvariantCulture);
                }
                else
                {
                    AdvertisementId = 0;
                }

                object configMethods;
                if (deviceInfo.Properties.TryGetValue("System.Devices.WiFiDirectServices.ServiceConfigMethods", out configMethods))
                {
                    ConfigMethods = Convert.ToUInt32(configMethods, CultureInfo.InvariantCulture);
                }
                else
                {
                    ConfigMethods = 0;
                }

                object serviceAddress;
                if (deviceInfo.Properties.TryGetValue("System.Devices.WiFiDirectServices.ServiceAddress", out serviceAddress))
                {
                    byte[] macBuffer = (byte[])serviceAddress;

                    ServiceAddress = "";
                    foreach (byte octet in macBuffer)
                    {
                        ServiceAddress += octet.ToString("X2", CultureInfo.InvariantCulture) + ":";
                    }
                    if (ServiceAddress.Length > 0)
                    {
                        ServiceAddress = ServiceAddress.Substring(0, ServiceAddress.Length - 1);
                    }
                }
                else
                {
                    ServiceAddress = "XX:XX:XX:XX:XX:XX";
                }

                ServiceInfo = "";
                object serviceInfo;
                if (deviceInfo.Properties.TryGetValue("System.Devices.WiFiDirectServices.ServiceInformation", out serviceInfo))
                {
                    if (serviceInfo != null)
                    {
                        IBuffer serviceInfoBuffer;
                        using (var tempStream = new Windows.Storage.Streams.InMemoryRandomAccessStream())
                        {
                            using (var serviceInfoDataWriter = new DataWriter(tempStream))
                            {
                                serviceInfoDataWriter.WriteBytes((Byte[])serviceInfo);
                                serviceInfoBuffer = serviceInfoDataWriter.DetachBuffer();
                            }
                        }

                        using (DataReader serviceInfoDataReader = DataReader.FromBuffer(serviceInfoBuffer))
                        {
                            ServiceInfo = serviceInfoDataReader.ReadString(serviceInfoBuffer.Length);
                        }
                    }
                }
            }

            // Expose properties from the discovered device
            public string ServiceName { get; private set; }
            public string ServiceAddress { get; private set; }
            public UInt32 AdvertisementId { get; private set; }
            public UInt32 ConfigMethods { get; private set; }
            public string ServiceInfo { get; private set; }

            /// <summary>
            /// Calls WiFiDirectService.FromId to open the seeker session
            /// Caller can then use this.Service to modify settings before connecting
            /// </summary>
            public async Task OpenSessionAsync()
            {
                ThrowIfDisposed();

                // This wrapper can only be used for FromIdAsync once
                if (service != null)
                {
                    throw new Exception("Attempted to connect to discovered service a second time!");
                }

                manager.NotifyUser("Open session...", NotifyType.StatusMessage);

                try
                {
                    // NOTE: This MUST be called from the UI thread
                    service = await WiFiDirectService.FromIdAsync(deviceInfo.Id);
                    service.SessionDeferred += OnSessionDeferred;
                }
                catch (Exception ex)
                {
                    manager.NotifyUser("FromIdAsync Failed: " + ex.Message, NotifyType.ErrorMessage);
                }
            }

            /// <summary>
            /// Call this to set options before doing the actual connect
            /// </summary>
            /// <param name="preferGO">If true, this peer will prefer to be the group owner (GO) of the connection</param>
            /// <param name="sessionInfo">Optional 144 byte buffer to send with the request to the advertising peer</param>
            public async Task SetServiceOptionsAsync(bool preferGO, string sessionInfo)
            {
                ThrowIfDisposed();

                // Must call FromIdAsync first
                if (service == null)
                {
                    await OpenSessionAsync();
                }
                
                service.PreferGroupOwnerMode = preferGO;

                if (sessionInfo != "")
                {
                    using (var sessionInfoDataWriter = new DataWriter(new InMemoryRandomAccessStream()))
                    {
                        sessionInfoDataWriter.WriteString(sessionInfo);
                        service.SessionInfo = sessionInfoDataWriter.DetachBuffer();
                    }
                }
                else
                {
                    service.SessionInfo = null;
                }
            }

            /// <summary>
            /// Calls service.GetProvisioningInfoAsync to begin provision discovery and
            /// determine if a PIN will be needed to connect
            /// </summary>
            /// <param name="configMethod">Selected Config Method to use, based on supported config methods of discovered device</param>
            public async Task<WiFiDirectServiceProvisioningInfo> GetProvisioningInfoAsync(
                WiFiDirectServiceConfigurationMethod configMethod
                )
            {
                WiFiDirectServiceProvisioningInfo provisioningInfo = null;
                ThrowIfDisposed();

                // Must call FromIdAsync first
                if (service == null)
                {
                    await OpenSessionAsync();
                }

                manager.NotifyUser("Get provisioning info...", NotifyType.StatusMessage);

                try
                {
                    provisioningInfo = await service.GetProvisioningInfoAsync(configMethod);
                    
                    if (provisioningInfo.IsGroupFormationNeeded)
                    {
                        if (provisioningInfo.SelectedConfigurationMethod == WiFiDirectServiceConfigurationMethod.PinDisplay)
                        {
                            // UI should display the PIN and pass it into the call to Connect
                            manager.NotifyUser("Group Formation is needed, should DISPLAY PIN", NotifyType.StatusMessage);
                        }
                        else if (provisioningInfo.SelectedConfigurationMethod == WiFiDirectServiceConfigurationMethod.PinEntry)
                        {
                            // UI should take PIN as input and pass it into the call to Connect
                            manager.NotifyUser("Group Formation is needed, should ENTER PIN", NotifyType.StatusMessage);
                        }
                        else
                        {
                            manager.NotifyUser("Group Formation is needed, no PIN required", NotifyType.StatusMessage);
                        }
                    }
                    else
                    {
                        manager.NotifyUser("Group Formation is not needed", NotifyType.StatusMessage);
                    }
                }
                catch (Exception ex)
                {
                    // This could fail due to driver or network issues
                    manager.NotifyUser("GetProvisioningInfoAsync Failed: " + ex.Message, NotifyType.ErrorMessage);
                }

                return provisioningInfo;
            }

            /// <summary>
            /// Calls service.ConnectAsync() to connect the session
            /// If the session is not already opened, it calls OpenSessionAsync
            /// </summary>
            /// <param name="pin">Optional PIN if needed</param>
            public async void ConnectAsync(string pin = "")
            {
                ThrowIfDisposed();

                // Must call FromIdAsync first
                if (service == null)
                {
                    await OpenSessionAsync();
                }

                WiFiDirectServiceSession session = null;

                manager.NotifyUser("Connecting...", NotifyType.StatusMessage);

                try
                {
                    if (pin.Length > 0)
                    {
                        session = await service.ConnectAsync(pin);
                    }
                    else
                    {
                        session = await service.ConnectAsync();
                    }
                    
                    manager.NotifyUser("Done Connecting", NotifyType.StatusMessage);

                    // Now we are done with this WiFiDirectService instance
                    // Clear state so a new connection can be started
                    service.SessionDeferred -= OnSessionDeferred;
                    service = null;

                    SessionWrapper sessionWrapper = new SessionWrapper(session, manager);
                    manager.AddSession(sessionWrapper);
                }
                catch (Exception ex)
                {
                    // This could fail due to driver or network issues
                    manager.NotifyUser("ConnectAsync Failed: " + ex.Message, NotifyType.ErrorMessage);
                }
            }

            private void OnSessionDeferred(WiFiDirectService sender, WiFiDirectServiceSessionDeferredEventArgs args)
            {
                string deferredSessionInfo = "";
                if (args.DeferredSessionInfo != null && args.DeferredSessionInfo.Length > 0)
                {
                    using (DataReader sessionInfoDataReader = DataReader.FromBuffer(args.DeferredSessionInfo))
                    {
                        deferredSessionInfo = sessionInfoDataReader.ReadString(args.DeferredSessionInfo.Length);
                    }
                }

                manager.NotifyUser("Session Connection was deferred... (" + deferredSessionInfo + ")", NotifyType.StatusMessage);
            }

            #region Dispose
            bool disposed = false;

            ~DiscoveredDeviceWrapper()
            {
                Dispose(false);
            }

            public void Dispose()
            {
                Dispose(true);
                GC.SuppressFinalize(this);
            }

            protected virtual void Dispose(bool disposing)
            {
                if (disposed)
                {
                    return;
                }

                if (disposing)
                {
                    if (service != null)
                    {
                        service.SessionDeferred -= OnSessionDeferred;
                    }
                }

                disposed = true;
            }

            private void ThrowIfDisposed()
            {
                if (disposed)
                {
                    throw new ObjectDisposedException("DiscoveredDeviceWrapper");
                }
            }
            #endregion
        }

        public class SessionWrapper : IDisposable
        {
            private WiFiDirectServiceSession session;
            // Used to update main state
            private WiFiDirectServiceManager manager = null;

            // Stream Socket Listeners are created when locally opening TCP ports
            private IList<StreamSocketListenerWrapper> streamSocketListeners = new List<StreamSocketListenerWrapper>();

            // Keep a list of connected sockets
            private IList<SocketWrapper> socketList = new List<SocketWrapper>();
            
            // Used to wait for the session to close before cleaning up the wrapper
            private AutoResetEvent sessionClosedEvent = new AutoResetEvent(false);

            public SessionWrapper(
                WiFiDirectServiceSession session,
                WiFiDirectServiceManager manager)
            {
                this.session = session;
                this.manager = manager;

                this.session.SessionStatusChanged += OnSessionStatusChanged;
                this.session.RemotePortAdded += OnRemotePortAdded;
            }

            public WiFiDirectServiceSession Session
            {
                get { return session; }
            }

            public uint AdvertisementId
            {
                get { return session.AdvertisementId; }
            }
            public uint SessionId
            {
                get { return session.SessionId; }
            }

            public string ServiceAddress
            {
                get { return session.ServiceAddress; }
            }

            public string SessionAddress
            {
                get { return session.SessionAddress; }
            }

            public IList<SocketWrapper> SocketList
            {
                get { return socketList; }
            }

            public void Close()
            {
                ThrowIfDisposed();

                foreach (var socket in socketList)
                {
                    socket.Dispose();
                }
                socketList.Clear();

                session.Dispose();

                // Wait for status change
                // NOTE: this should complete in 5 seconds under normal circumstances
                if (!sessionClosedEvent.WaitOne(60000))
                {
                    throw new Exception("Timed out waiting for session to close");
                }
            }

            public async void AddStreamSocketListenerAsync(UInt16 port)
            {
                ThrowIfDisposed();

                manager.NotifyUser("Adding stream socket listener for TCP port " + port, NotifyType.StatusMessage);

                var endpointPairCollection = session.GetConnectionEndpointPairs();
                
                // Create listener for socket connection (and add to list to cleanup later)
                StreamSocketListener listenerSocket = new StreamSocketListener();
                StreamSocketListenerWrapper listenerWrapper = new StreamSocketListenerWrapper(
                    manager,
                    listenerSocket,
                    this
                    );

                try
                {
                    manager.NotifyUser("BindEndpointAsync...", NotifyType.StatusMessage);
                    await listenerSocket.BindEndpointAsync(endpointPairCollection[0].LocalHostName, Convert.ToString(port, CultureInfo.InvariantCulture));
                    manager.NotifyUser("BindEndpointAsync Done", NotifyType.StatusMessage);

                    manager.NotifyUser("AddStreamSocketListenerAsync...", NotifyType.StatusMessage);
                    await session.AddStreamSocketListenerAsync(listenerSocket);
                    manager.NotifyUser("AddStreamSocketListenerAsync Done", NotifyType.StatusMessage);
                }
                catch (Exception ex)
                {
                    manager.NotifyUser("AddStreamSocketListenerAsync Failed: " + ex.Message, NotifyType.ErrorMessage);
                }

                streamSocketListeners.Add(listenerWrapper);
            }

            public async void AddDatagramSocketAsync(UInt16 port)
            {
                ThrowIfDisposed();

                manager.NotifyUser("Adding stream socket listener for UDP port " + port, NotifyType.StatusMessage);

                var endpointPairCollection = session.GetConnectionEndpointPairs();

                DatagramSocket socket = new DatagramSocket();
                // Socket is "read-only", cannot send data but can receive data
                // Expectation is that application starts a listening socket, then remote device sends data
                SocketWrapper socketWrapper = new SocketWrapper(manager, null, socket, false);

                try
                {
                    // Bind UDP socket for receiving messages (peer should call connect and send messages to this socket)
                    manager.NotifyUser("BindEndpointAsync...", NotifyType.StatusMessage);
                    await socket.BindEndpointAsync(endpointPairCollection[0].LocalHostName, Convert.ToString(port, CultureInfo.InvariantCulture));
                    manager.NotifyUser("BindEndpointAsync Done", NotifyType.StatusMessage);

                    manager.NotifyUser("AddDatagramSocketAsync...", NotifyType.StatusMessage);
                    await session.AddDatagramSocketAsync(socket);
                    manager.NotifyUser("AddDatagramSocketAsync Done", NotifyType.StatusMessage);

                    // Update manager so UI can add to list
                    manager.AddSocket(socketWrapper);
                }
                catch (Exception ex)
                {
                    manager.NotifyUser("AddDatagramSocketAsync Failed: " + ex.Message, NotifyType.ErrorMessage);
                }

                socketList.Add(socketWrapper);
            }

            // This will fire when the connected peer attempts to open a port for this connection
            // The peer should start a stream socket listener (for TCP)
            private async void OnRemotePortAdded(WiFiDirectServiceSession sender, WiFiDirectServiceRemotePortAddedEventArgs args)
            {
                try
                {
                    ThrowIfDisposed();

                    var endpointPairCollection = args.EndpointPairs;
                    var protocol = args.Protocol;
                    if (endpointPairCollection.Count == 0)
                    {
                        manager.NotifyUser("No endpoint pairs for remote port added event", NotifyType.ErrorMessage);
                        return;
                    }

                    manager.NotifyUser(String.Format("{0} Port {1} Added by remote peer",
                        (protocol == WiFiDirectServiceIPProtocol.Tcp) ? "TCP" : ((protocol == WiFiDirectServiceIPProtocol.Udp) ? "UDP" : "???"),
                        endpointPairCollection[0].RemoteServiceName
                        ),
                        NotifyType.StatusMessage
                        );

                    SocketWrapper socketWrapper = null;

                    if (args.Protocol == WiFiDirectServiceIPProtocol.Tcp)
                    {
                        // Connect to the stream socket listener
                        StreamSocket streamSocket = new StreamSocket();
                        socketWrapper = new SocketWrapper(manager, streamSocket, null, true);

                        manager.NotifyUser("Connecting to stream socket...", NotifyType.StatusMessage);
                        await streamSocket.ConnectAsync(endpointPairCollection[0]);
                        // Start receiving messages recursively
                        var rcv = socketWrapper.ReceiveMessageAsync();
                        manager.NotifyUser("Stream socket connected", NotifyType.StatusMessage);
                    }
                    else if (args.Protocol == WiFiDirectServiceIPProtocol.Udp)
                    {
                        // Connect a socket over UDP
                        DatagramSocket datagramSocket = new DatagramSocket();
                        socketWrapper = new SocketWrapper(manager, null, datagramSocket, true);

                        manager.NotifyUser("Connecting to datagram socket...", NotifyType.StatusMessage);
                        await datagramSocket.ConnectAsync(endpointPairCollection[0]);
                        manager.NotifyUser("Datagram socket connected", NotifyType.StatusMessage);
                    }
                    else
                    {
                        manager.NotifyUser("Bad protocol for remote port added event", NotifyType.ErrorMessage);
                        return;
                    }

                    socketList.Add(socketWrapper);
                    // Update manager so UI can add to list
                    manager.AddSocket(socketWrapper);
                }
                catch (Exception ex)
                {
                    manager.NotifyUser("OnRemotePortAdded Failed: " + ex.Message, NotifyType.ErrorMessage);
                }
            }

            public void AddStreamSocketInternal(StreamSocket socket)
            {
                SocketWrapper socketWrapper = new SocketWrapper(manager, socket, null, true);
                
                // Start receiving messages recursively
                var rcv = socketWrapper.ReceiveMessageAsync();

                socketList.Add(socketWrapper);
                // Update manager so UI can add to list
                manager.AddSocket(socketWrapper);
            }

            private void OnSessionStatusChanged(WiFiDirectServiceSession sender, object args)
            {
                try
                {
                    ThrowIfDisposed();

                    manager.NotifyUser("Session Status Changed to " + session.Status.ToString(), NotifyType.StatusMessage);

                    if (session.Status == WiFiDirectServiceSessionStatus.Closed)
                    {
                        sessionClosedEvent.Set();
                        // Cleanup
                        manager.RemoveSession(this);
                    }
                }
                catch (Exception ex)
                {
                    manager.NotifyUser("OnSessionStatusChanged Failed: " + ex.Message, NotifyType.ErrorMessage);
                }
            }

            #region Dispose
            bool disposed = false;

            ~SessionWrapper()
            {
                Dispose(false);
            }

            public void Dispose()
            {
                Dispose(true);
                GC.SuppressFinalize(this);
            }

            protected virtual void Dispose(bool disposing)
            {
                if (disposed)
                {
                    return;
                }

                if (disposing)
                {
                    // This will close the session
                    session.Dispose();

                    // Remove event handlers
                    session.SessionStatusChanged -= OnSessionStatusChanged;
                    session.RemotePortAdded -= OnRemotePortAdded;

                    foreach (var listener in streamSocketListeners)
                    {
                        listener.Dispose();
                    }
                    streamSocketListeners.Clear();

                    foreach (var socket in socketList)
                    {
                        socket.Dispose();
                    }
                    socketList.Clear();
                    
                    sessionClosedEvent.Dispose();
                }

                disposed = true;
            }

            private void ThrowIfDisposed()
            {
                if (disposed)
                {
                    throw new ObjectDisposedException("SessionWrapper");
                }
            }
            #endregion
        }

        public class StreamSocketListenerWrapper : IDisposable
        {
            private StreamSocketListener streamSocketListener;
            // Used to update main state
            private WiFiDirectServiceManager manager = null;

            private SessionWrapper session = null;

            public StreamSocketListenerWrapper(
                WiFiDirectServiceManager manager,
                StreamSocketListener streamSocketListener,
                SessionWrapper session
                )
            {
                this.streamSocketListener = streamSocketListener;
                this.session = session;
                this.manager = manager;

                this.streamSocketListener.ConnectionReceived += OnConnectionReceived;
            }

            private void OnConnectionReceived(StreamSocketListener sender, StreamSocketListenerConnectionReceivedEventArgs args)
            {
                manager.NotifyUser("Connection received for TCP Port " + sender.Information.LocalPort, NotifyType.StatusMessage);

                session.AddStreamSocketInternal(args.Socket);
            }

            #region Dispose
            bool disposed = false;

            ~StreamSocketListenerWrapper()
            {
                Dispose(false);
            }

            public void Dispose()
            {
                Dispose(true);
                GC.SuppressFinalize(this);
            }

            protected virtual void Dispose(bool disposing)
            {
                if (disposed)
                {
                    return;
                }

                if (disposing)
                {
                    try
                    {
                        streamSocketListener.ConnectionReceived -= OnConnectionReceived;
                    }
                    catch (Exception)
                    { }

                    streamSocketListener.Dispose();
                }

                disposed = true;
            }

            private void ThrowIfDisposed()
            {
                if (disposed)
                {
                    throw new ObjectDisposedException("StreamSocketListenerWrapper");
                }
            }
            #endregion
        }

        // Generic wrapper for TCP (Stream) or UDP (Datagram) socket
        // For this sample, this just allows messages to be sent between connected peers,
        // real application would handle higher level logic over the connected socket(s)
        public class SocketWrapper : IDisposable
        {
            private StreamSocket streamSocket = null;
            private DatagramSocket datagramSocket = null;
            // Used to update main state
            private WiFiDirectServiceManager manager = null;
            
            private DataReader reader = null;
            private DataWriter writer;

            public SocketWrapper(
                WiFiDirectServiceManager manager,
                StreamSocket streamSocket = null,
                DatagramSocket datagramSocket = null,
                bool canSend = true)
            {
                this.streamSocket = streamSocket;
                this.datagramSocket = datagramSocket;
                this.manager = manager;

                if (streamSocket == null && datagramSocket == null)
                {
                    throw new Exception("Bad SocketWrapper parameters, must provide a TCP or UDP socket");
                }
                else if (streamSocket != null && datagramSocket != null)
                {
                    throw new Exception("Bad SocketWrapper parameters, got both TCP and UDP socket, expected only one");
                }
                else if (streamSocket != null)
                {
                    reader = new DataReader(streamSocket.InputStream);
                    if (canSend)
                    {
                        writer = new DataWriter(streamSocket.OutputStream);
                    }
                }
                else
                {
                    datagramSocket.MessageReceived += OnUDPMessageReceived;
                    if (canSend)
                    {
                        writer = new DataWriter(datagramSocket.OutputStream);
                    }
                }

                if (reader != null)
                {
                    reader.UnicodeEncoding = UnicodeEncoding.Utf8;
                    reader.ByteOrder = ByteOrder.LittleEndian;
                }

                if (writer != null)
                {
                    writer.UnicodeEncoding = UnicodeEncoding.Utf8;
                    writer.ByteOrder = ByteOrder.LittleEndian;
                }
            }

            public string Protocol
            {
                get
                {
                    if (streamSocket != null)
                    {
                        return "TCP";
                    }
                    else if (datagramSocket != null)
                    {
                        return "UDP";
                    }
                    else
                    {
                        return "???";
                    }
                }
            }

            public string Port
            {
                get
                {
                    if (streamSocket != null)
                    {
                        return streamSocket.Information.LocalPort;
                    }
                    else if (datagramSocket != null)
                    {
                        return datagramSocket.Information.LocalPort;
                    }
                    else
                    {
                        return "???";
                    }
                }
            }
            
            /// <summary>
            /// Send a string over the socket (TCP or UDP)
            /// Will send as a uint32 size followed by the message
            /// </summary>
            /// <param name="message">Message to send</param>
            public async Task SendMessageAsync(string message)
            {
                try
                {
                    ThrowIfDisposed();

                    if (writer == null)
                    {
                        manager.NotifyUser("Socket is unable to send messages (receive only socket).", NotifyType.ErrorMessage);
                    }
                    else
                    {
                        writer.WriteUInt32(writer.MeasureString(message));
                        writer.WriteString(message);
                        await writer.StoreAsync();

                        uint bytesSent = writer.MeasureString(message);

                        manager.NotifyUser(String.Format("Sent Message: \"{0}\", {1} bytes",
                                (message.Length > 32) ? message.Substring(0, 32) + "..." : message,
                                bytesSent
                                ),
                            NotifyType.StatusMessage
                            );
                    }
                }
                catch (Exception ex)
                {
                    manager.NotifyUser("SendMessageAsync Failed: " + ex.Message, NotifyType.ErrorMessage);
                }
            }

            /// <summary>
            /// Read strings sent over the TCP socket
            /// Reads the uint32 size then the actual string
            /// </summary>
            public async Task ReceiveMessageAsync()
            {
                await HandleReceivedMessage(this.reader);
            }

            /// <summary>
            /// Handle both TCP receive call and UDP event handler
            /// </summary>
            /// <param name="datareader"></param>
            /// <param name="load">Set to true when the datareader.LoadAsync needs to be called for TCP</param>
            /// <returns></returns>
            private async Task HandleReceivedMessage(DataReader datareader, bool load = true)
            {
                try
                {
                    ThrowIfDisposed();
                    uint bytesRead = 0;

                    if (load)
                    {
                        bytesRead = await datareader.LoadAsync(sizeof(uint));

                        if (bytesRead != sizeof(uint))
                        {
                            manager.NotifyUser("The underlying socket was closed before we were able to read the whole data.", NotifyType.ErrorMessage);
                            return;
                        }
                    }

                    if (!load || bytesRead > 0)
                    {
                        // Determine how long the string is.
                        uint messageLength = (uint)datareader.ReadUInt32();
                        
                        if (load)
                        {
                            bytesRead = await datareader.LoadAsync(messageLength);
                            if (bytesRead != messageLength)
                            {
                                manager.NotifyUser("The underlying socket was closed before we were able to read the whole data.", NotifyType.ErrorMessage);
                                return;
                            }
                        }

                        if ((!load && messageLength > 0) || bytesRead > 0)
                        {
                            // Decode the string.
                            string currentMessage = datareader.ReadString(messageLength);
                            uint bytesReceived = messageLength;

                            // Just print a message for now
                            manager.NotifyUser(String.Format("Received Message: \"{0}\", {1} bytes",
                                    (currentMessage.Length > 32) ? currentMessage.Substring(0, 32) + "..." : currentMessage,
                                    bytesReceived
                                    ),
                                NotifyType.StatusMessage
                                );

                            // TCP will need to call this again, UDP will get callbacks
                            if (load)
                            {
                                await HandleReceivedMessage(datareader, load);
                            }
                        }
                        else
                        {
                            manager.NotifyUser("ReceiveMessage 0 bytes loaded for message content.", NotifyType.ErrorMessage);
                        }
                    }
                    else
                    {
                        manager.NotifyUser("ReceiveMessage 0 bytes loaded for message content.", NotifyType.ErrorMessage);
                    }
                }
                catch (Exception ex)
                {
                    manager.NotifyUser("HandleReceivedMessage Failed: " + ex.Message, NotifyType.ErrorMessage);
                }
            }

            private async void OnUDPMessageReceived(DatagramSocket sender, DatagramSocketMessageReceivedEventArgs args)
            {
                try
                {
                    DataReader udpReader = args.GetDataReader();
                    udpReader.UnicodeEncoding = UnicodeEncoding.Utf8;
                    udpReader.ByteOrder = ByteOrder.LittleEndian;

                    await HandleReceivedMessage(udpReader, false);
                }
                catch (Exception ex)
                {
                    manager.NotifyUser("OnUDPMessageReceived Failed: " + ex.Message, NotifyType.ErrorMessage);
                }
            }

            #region Dispose
            bool disposed = false;

            ~SocketWrapper()
            {
                Dispose(false);
            }

            public void Dispose()
            {
                Dispose(true);
                GC.SuppressFinalize(this);
            }

            protected virtual void Dispose(bool disposing)
            {
                if (disposed)
                {
                    return;
                }

                if (disposing)
                {
                    if (streamSocket != null)
                    {
                        streamSocket.Dispose();
                    }

                    if (datagramSocket != null)
                    {
                        datagramSocket.Dispose();
                        datagramSocket.MessageReceived -= OnUDPMessageReceived;
                    }

                    if (writer != null)
                    {
                        writer.Dispose();
                    }

                    if (reader != null)
                    {
                        reader.Dispose();
                    }
                }

                disposed = true;
            }

            private void ThrowIfDisposed()
            {
                if (disposed)
                {
                    throw new ObjectDisposedException("SocketWrapper");
                }
            }
            #endregion
        }
    }
    
}