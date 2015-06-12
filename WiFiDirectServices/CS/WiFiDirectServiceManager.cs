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
using Windows.Devices.WiFiDirect.Services;
using Windows.Devices.Enumeration;
using Windows.UI.Core;
using Windows.Storage.Streams;
using System.Globalization;

namespace SDKTemplate
{
    namespace WiFiDirectServices
    {

        /// <summary>
        /// Common class to wrap all of the services objects and state behind the UI
        /// </summary>
        public class WiFiDirectServiceManager : IDisposable
        {
            private Object thisLock = new Object();

            private MainPage rootPage;

            // UI Integration
            // Keep track of known UI frames and update them when we have any changes
            // NOTE: UI pages may not exist and the UI will need to update itself when it initializes
            private Scenario1 scenario1 = null;
            private Scenario2 scenario2 = null;
            private Scenario3 scenario3 = null;
            private Scenario4 scenario4 = null;
            private Scenario5 scenario5 = null;

            // Keep list of all advertised services currently active
            private IList<AdvertisementWrapper> advertisers = new List<AdvertisementWrapper>();

            // Keep track of all devices found during previous discovery
            private IList<DiscoveredDeviceWrapper> discoveredDevices = new List<DiscoveredDeviceWrapper>();

            // Keep track of all sessions that are connected to this device
            // NOTE: it may make sense to track sessions per-advertiser and a second list for the seeker, but this sample keeps a global list
            private IList<SessionWrapper> connectedSessions = new List<SessionWrapper>();

            // Keep track of the selected session for the UI
            // The UI will choose this session from the session list and use it for sending/receiving data
            private int selectedSession = -1;

            // Singleton
            private static WiFiDirectServiceManager instance;
            private WiFiDirectServiceManager()
            {
                instance = this;
                this.rootPage = MainPage.Current;
            }
            
            // Singleton access
            public static WiFiDirectServiceManager Instance
            {
                get
                {
                    if (instance == null)
                    {
                        instance = new WiFiDirectServiceManager();
                    }
                    return instance;
                }
            }

            public async void NotifyUser(string message, NotifyType type)
            {
                // Always dispatch to UI thread
                await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    rootPage.NotifyUser(message, type);
                });
            }

            #region UI Page Accessors
            public MainPage RootPage
            {
                get
                {
                    if (rootPage == null)
                    {
                        rootPage = MainPage.Current;
                    }
                    return rootPage;
                }
            }

            public Scenario1 Scenario1
            {
                get { return scenario1; }
                set { scenario1 = value; }
            }

            public Scenario2 Scenario2
            {
                get { return scenario2; }
                set { scenario2 = value; }
            }

            public Scenario3 Scenario3
            {
                get { return scenario3; }
                set { scenario3 = value; }
            }

            public Scenario4 Scenario4
            {
                get { return scenario4; }
                set { scenario4 = value; }
            }

            public Scenario5 Scenario5
            {
                get { return scenario5; }
                set { scenario5 = value; }
            }
            #endregion

            public IList<AdvertisementWrapper> Advertisers
            {
                get { return advertisers; }
            }

            public SessionWrapper CurrentSession
            {
                get
                {
                    if (selectedSession >= 0 && selectedSession < connectedSessions.Count)
                    {
                        return connectedSessions[selectedSession];
                    }
                    return null;
                }
            }

            public IList<DiscoveredDeviceWrapper> DiscoveredDevices
            {
                get { return discoveredDevices; }
            }
            
            public IList<SessionWrapper> ConnectedSessionList
            {
                get { return connectedSessions; }
            }

            #region Advertiser
            public void StartAdvertisement(
                string serviceName,
                bool autoAccept,
                bool preferGO,
                string pin,
                IList<WiFiDirectServiceConfigurationMethod> configMethods,
                WiFiDirectServiceStatus status,
                uint customStatus,
                string serviceInfo,
                string deferredServiceInfo,
                IList<String> prefixList
                )
            {
                ThrowIfDisposed();
                
                // Create Advertiser object for the service
                // NOTE: service name is internally limited to up to 255 bytes in UTF-8 encoding
                // Valid characters include alpha-numeric, '.', '-', and any multi-byte character
                // characters a-z, A-Z are case-insensitive when discovering services
                WiFiDirectServiceAdvertiser advertiser = new WiFiDirectServiceAdvertiser(serviceName);

                // Auto-accept services will connect without interaction from advertiser
                // NOTE: if the config method used for a connection requires a PIN, then the advertiser will have to accept the connection
                advertiser.AutoAcceptSession = autoAccept;

                // Set the Group Owner intent to a large value so that the advertiser will try to become the group owner (GO)
                // NOTE: The GO of a P2P connection can connect to multiple clients while the client can connect to a single GO only
                advertiser.PreferGroupOwnerMode = preferGO;

                // Default status is "Available", but services may use a custom status code (value > 1) if applicable
                advertiser.ServiceStatus = status;
                advertiser.CustomServiceStatusCode = customStatus;

                // Service information can be up to 65000 bytes.
                // Service Seeker may explicitly discover this by specifying a short buffer that is a subset of this buffer.
                // If seeker portion matches, then entire buffer is returned, otherwise, the service information is not returned to the seeker
                // This sample uses a string for the buffer but it can be any data
                if (serviceInfo != null && serviceInfo.Length > 0)
                {
                    using (var tempStream = new Windows.Storage.Streams.InMemoryRandomAccessStream())
                    {
                        using (var serviceInfoDataWriter = new Windows.Storage.Streams.DataWriter(tempStream))
                        {
                            serviceInfoDataWriter.WriteString(serviceInfo);
                            advertiser.ServiceInfo = serviceInfoDataWriter.DetachBuffer();
                        }
                    }
                }
                else
                {
                    advertiser.ServiceInfo = null;
                }

                // This is a buffer of up to 144 bytes that is sent to the seeker in case the connection is "deferred" (i.e. not auto-accepted)
                // This buffer will be sent when auto-accept is false, or if a PIN is required to complete the connection
                // For the sample, we use a string, but it can contain any data
                if (deferredServiceInfo != null && deferredServiceInfo.Length > 0)
                {
                    using (var tempStream = new Windows.Storage.Streams.InMemoryRandomAccessStream())
                    {
                        using (var deferredSessionInfoDataWriter = new Windows.Storage.Streams.DataWriter(tempStream))
                        {
                            deferredSessionInfoDataWriter.WriteString(deferredServiceInfo);
                            advertiser.DeferredSessionInfo = deferredSessionInfoDataWriter.DetachBuffer();
                        }
                    }
                }
                else
                {
                    advertiser.DeferredSessionInfo = null;
                }

                // The advertiser supported configuration methods
                // Valid values are PIN-only (either keypad entry, display, or both), or PIN (keypad entry, display, or both) and WFD Services default
                // WFD Services Default config method does not require explicit PIN entry and offers a more seamless connection experience
                // Typically, an advertiser will support PIN display (and WFD Services Default), and a seeker will connect with either PIN entry or WFD Services Default
                if (configMethods != null)
                {
                    advertiser.PreferredConfigurationMethods.Clear();
                    foreach (var configMethod in configMethods)
                    {
                        advertiser.PreferredConfigurationMethods.Add(configMethod);
                    }
                }

                // Advertiser may also be discoverable by a prefix of the service name. Must explicitly specify prefixes allowed here.
                if (prefixList != null && prefixList.Count > 0)
                {
                    advertiser.ServiceNamePrefixes.Clear();
                    foreach (var prefix in prefixList)
                    {
                        advertiser.ServiceNamePrefixes.Add(prefix);
                    }
                }

                // For this sample, we wrap the advertiser in our own object which handles the advertiser events
                AdvertisementWrapper advertiserWrapper = new AdvertisementWrapper(advertiser, this, pin);
                
                AddAdvertiser(advertiserWrapper);

                RootPage.NotifyUser("Starting service...", NotifyType.StatusMessage);

                try
                {
                    // This may fail if the driver is unable to handle the request or if services is not supported
                    // NOTE: this must be called from the UI thread of the app
                    advertiser.Start();
                }
                catch (Exception ex)
                {
                    RootPage.NotifyUser(String.Format(CultureInfo.InvariantCulture, "Failed to start service: {0}", ex.Message), NotifyType.ErrorMessage);
                    throw;
                }
            }

            public void UnpublishService(int index)
            {
                ThrowIfDisposed();

                AdvertisementWrapper advertiser = null;

                lock (thisLock)
                {
                    if (index > advertisers.Count - 1)
                    {
                        throw new IndexOutOfRangeException("Attempted to stop service not found in list");
                    }

                    advertiser = advertisers[index];
                }

                try
                {
                    advertiser.Advertiser.Stop();
                }
                catch (Exception ex)
                {
                    RootPage.NotifyUser("Stop Advertisement Failed: " + ex.Message, NotifyType.ErrorMessage);
                }
            }
            #endregion

            #region Seeker
            public async void DiscoverServicesAsync(string serviceName, string requestedServiceInfo)
            {
                try
                {
                    ThrowIfDisposed();

                    RootPage.NotifyUser("Discover services... (name='" + serviceName + "', requestedInfo='" + requestedServiceInfo + "')", NotifyType.StatusMessage);

                    // Clear old results
                    discoveredDevices.Clear();

                    // Discovery depends on whether service information is requested
                    string serviceSelector = "";
                    if (requestedServiceInfo == "")
                    {
                        // Just search by name
                        serviceSelector = WiFiDirectService.GetSelector(serviceName);
                    }
                    else
                    {
                        using (var serviceInfoDataWriter = new DataWriter(new InMemoryRandomAccessStream()))
                        {
                            serviceInfoDataWriter.WriteString(requestedServiceInfo);
                            // Discover by name and try to discover service information
                            serviceSelector = WiFiDirectService.GetSelector(serviceName, serviceInfoDataWriter.DetachBuffer());
                        }
                    }

                    List<string> additionalProperties = new List<string>();
                    additionalProperties.Add("System.Devices.WiFiDirectServices.ServiceAddress");
                    additionalProperties.Add("System.Devices.WiFiDirectServices.ServiceName");
                    additionalProperties.Add("System.Devices.WiFiDirectServices.ServiceInformation");
                    additionalProperties.Add("System.Devices.WiFiDirectServices.AdvertisementId");
                    additionalProperties.Add("System.Devices.WiFiDirectServices.ServiceConfigMethods");

                    // Note: This sample demonstrates finding services with FindAllAsync, which does a discovery and returns a list
                    // It is also possible to use DeviceWatcher to receive updates as soon as services are found and to continue the discovery until it is stopped
                    // See the DeviceWatcher sample for an example on how to use that class instead of DeviceInformation.FindAllAsync
                    DeviceInformationCollection deviceInfoCollection = await DeviceInformation.FindAllAsync(serviceSelector, additionalProperties);
                    
                    if (deviceInfoCollection != null && deviceInfoCollection.Count > 0)
                    {
                        RootPage.NotifyUser("Done discovering services, found " + deviceInfoCollection.Count + " services", NotifyType.StatusMessage);

                        foreach (var device in deviceInfoCollection)
                        {
                            discoveredDevices.Add(new DiscoveredDeviceWrapper(device, this));
                        }
                    }
                    else
                    {
                        RootPage.NotifyUser("Done discovering services, No services found", NotifyType.StatusMessage);
                    }

                    // Update UI list
                    if (scenario3 != null)
                    {
                        scenario3.UpdateDiscoveryList(discoveredDevices);
                    }
                }
                catch (Exception ex)
                {
                    RootPage.NotifyUser(String.Format("Failed to discover services: {0}", ex.Message), NotifyType.ErrorMessage);
                    throw ex;
                }
            }
            #endregion

            #region ListManagement
            private void AddAdvertiser(AdvertisementWrapper advertiser)
            {
                lock (thisLock)
                {
                    advertisers.Add(advertiser);
                }

                // Update UI to add this to list
                if (scenario2 != null)
                {
                    scenario2.AddAdvertiser(advertiser);
                }
            }

            private void RemoveAdvertiser(int index)
            {
                lock (thisLock)
                {
                    advertisers.RemoveAt(index);
                }

                // Update UI to remove from list
                if (scenario2 != null)
                {
                    scenario2.RemoveAdvertiser(index);
                }
            }

            public void RemoveAdvertiser(AdvertisementWrapper advertiser)
            {
                int index = 0;
                bool fFound = false;

                lock (thisLock)
                {
                    // Lookup the index
                    foreach (var a in advertisers)
                    {
                        if (a.InternalId == advertiser.InternalId)
                        {
                            fFound = true;
                            break;
                        }
                        ++index;
                    }
                }

                if (fFound)
                {
                    RemoveAdvertiser(index);
                }
                else
                {
                    NotifyUser("Advertiser not found in list", NotifyType.ErrorMessage);
                }
            }

            public void AddSessionRequest(WiFiDirectServiceSessionRequest request, AdvertisementWrapper advertiser)
            {
                // Update UI to add this to list
                if (scenario2 != null)
                {
                    scenario2.AddSessionRequest(request, advertiser);
                }
            }

            public void RemoveSessionRequest(WiFiDirectServiceSessionRequest request, AdvertisementWrapper advertiser)
            {
                // Update UI to remove from list
                if (scenario2 != null)
                {
                    scenario2.RemoveSessionRequest(request, advertiser);
                }
            }
            
            public void AdvertiserStatusChanged(AdvertisementWrapper advertiser)
            {
                if (advertiser.Advertiser.AdvertisementStatus == WiFiDirectServiceAdvertisementStatus.Aborted ||
                    advertiser.Advertiser.AdvertisementStatus == WiFiDirectServiceAdvertisementStatus.Stopped)
                {
                    RemoveAdvertiser(advertiser);
                }
                else
                {
                    if (scenario2 != null)
                    {
                        scenario2.UpdateAdvertiserStatus(advertiser);
                    }
                }
            }
            
            public void AddSession(SessionWrapper session)
            {
                lock (thisLock)
                {
                    connectedSessions.Add(session);
                }

                // Update UI to add this to list
                if (scenario2 != null)
                {
                    scenario2.AddSession(session);
                }
                if (scenario4 != null)
                {
                    scenario4.AddSession(session);
                }

                if (scenario3 != null)
                {
                    scenario3.SessionConnected();
                }
            }

            public void RemoveSession(int index)
            {
                lock (thisLock)
                {
                    // Invalidate selection
                    selectedSession = -1;
                    connectedSessions.RemoveAt(index);
                }

                // Update UI to remove from list
                if (scenario2 != null)
                {
                    scenario2.RemoveSession(index);
                }
                if (scenario4 != null)
                {
                    scenario4.RemoveSession(index);
                }
            }

            public void RemoveSession(SessionWrapper session)
            {
                int index = 0;
                bool fFound = false;

                lock (thisLock)
                {
                    // Lookup the index
                    foreach (var s in connectedSessions)
                    {
                        if (s.Session.SessionId == session.Session.SessionId &&
                            s.Session.SessionAddress == session.Session.SessionAddress &&
                            s.Session.AdvertisementId == session.Session.AdvertisementId &&
                            s.Session.ServiceAddress == session.Session.ServiceAddress)
                        {
                            fFound = true;
                            break;
                        }
                        ++index;
                    }
                }

                if (fFound)
                {
                    RemoveSession(index);
                }
                else
                {
                    NotifyUser("Session not found in list", NotifyType.ErrorMessage);
                }
            }

            public void CloseSession(int index)
            {
                SessionWrapper session = null;

                // First remove from list
                lock (thisLock)
                {
                    if (index > connectedSessions.Count - 1)
                    {
                        throw new IndexOutOfRangeException("Attempted to close session not found in list");
                    }

                    session = connectedSessions[index];
                    // Session will remove self from list when its status changes
                }

                // Close session by disposing underlying WiFiDirectServiceSession
                session.Close();
                RootPage.NotifyUser("Closed Session", NotifyType.StatusMessage);
            }

            public void SelectSession(int index)
            {
                lock (thisLock)
                {
                    if (index > connectedSessions.Count - 1)
                    {
                        throw new IndexOutOfRangeException("Attempted to select session not found in list");
                    }
                    selectedSession = index;
                }
            }

            public void AddSocket(SocketWrapper socket)
            {
                // Update UI to add this to list
                if (scenario5 != null)
                {
                    scenario5.AddSocket(socket);
                }
            }
            #endregion

            #region Dispose
            bool disposed = false;
            
            ~WiFiDirectServiceManager()
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
                    foreach (var advertiser in advertisers)
                    {
                        advertiser.Dispose();
                    }
                    advertisers.Clear();

                    foreach (var device in discoveredDevices)
                    {
                        device.Dispose();
                    }
                    discoveredDevices.Clear();

                    foreach (var session in connectedSessions)
                    {
                        session.Dispose();
                    }
                    connectedSessions.Clear();
                }

                disposed = true;
            }

            private void ThrowIfDisposed()
            {
                if (disposed)
                {
                    throw new ObjectDisposedException("WiFiDirectServiceManager");
                }
            }
            #endregion
        }
    }
}