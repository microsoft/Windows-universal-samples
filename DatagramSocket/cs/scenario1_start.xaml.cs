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

using SDKTemplate;
using System;
using System.Collections.Generic;
using Windows.ApplicationModel.Core;
using Windows.Networking;
using Windows.Networking.Connectivity;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace DatagramSocketSample
{
    /// <summary>
    /// A page for first scenario.
    /// </summary>
    public sealed partial class Scenario1 : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        private MainPage rootPage = MainPage.Current;

        // List containing all available local HostName endpoints
        private List<LocalHostItem> localHostItems = new List<LocalHostItem>();

        public Scenario1()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            BindToAny.IsChecked = true;
            PopulateAdapterList();
        }

        private void BindToAny_Checked(object sender, RoutedEventArgs e)
        {
            AdapterList.IsEnabled = false;
        }

        private void BindToAny_Unchecked(object sender, RoutedEventArgs e)
        {
            AdapterList.IsEnabled = true;
        }

        /// <summary>
        /// This is the click handler for the 'StartListener' button.
        /// </summary>
        /// <param name="sender">Object for which the event was generated.</param>
        /// <param name="e">Event's parameters.</param>
        private async void StartListener_Click(object sender, RoutedEventArgs e)
        {
            if (String.IsNullOrEmpty(ServiceNameForListener.Text))
            {
                rootPage.NotifyUser("Please provide a service name.", NotifyType.ErrorMessage);
                return;
            }

            if (CoreApplication.Properties.ContainsKey("listener"))
            {
                rootPage.NotifyUser(
                    "This step has already been executed. Please move to the next one.", 
                    NotifyType.ErrorMessage);
                return;
            }

            CoreApplication.Properties.Remove("serverAddress");

            DatagramSocket listener = new DatagramSocket();
            listener.MessageReceived += MessageReceived;

            if (!String.IsNullOrWhiteSpace(InboundBufferSize.Text))
            {
                uint inboundBufferSize = 0;
                if (!uint.TryParse(InboundBufferSize.Text, out inboundBufferSize))
                {
                    rootPage.NotifyUser(
                        "Please provide a positive numeric Inbound buffer size.", 
                        NotifyType.ErrorMessage);
                    return;
                }

                try
                {
                    // Running both client and server on localhost will most likely not reach the buffer limit.
                    listener.Control.InboundBufferSizeInBytes = inboundBufferSize;
                }
                catch (ArgumentException)
                {
                    rootPage.NotifyUser("Please provide a valid Inbound buffer size.", NotifyType.ErrorMessage);
                    return;
                }
            }

            LocalHostItem selectedLocalHost = null; 
            if ((BindToAddress.IsChecked == true) || (BindToAdapter.IsChecked == true))
            {
                selectedLocalHost = (LocalHostItem)AdapterList.SelectedItem;
                if (selectedLocalHost == null)
                {
                    rootPage.NotifyUser("Please select an address / adapter.", NotifyType.ErrorMessage);
                    return;
                }

                // The user selected an address. For demo purposes, we ensure that connect will be using the same 
                // address.
                CoreApplication.Properties.Add("serverAddress", selectedLocalHost.LocalHost.CanonicalName);
            }

            // Save the socket, so subsequent steps can use it.
            CoreApplication.Properties.Add("listener", listener);

            // Start listen operation.
            try
            {
                if (BindToAny.IsChecked == true)
                {
                    // Don't limit traffic to an address or an adapter.
                    await listener.BindServiceNameAsync(ServiceNameForListener.Text);
                    rootPage.NotifyUser("Listening", NotifyType.StatusMessage);
                }
                else if (BindToAddress.IsChecked == true)
                {
                    // Try to bind to a specific address.
                    await listener.BindEndpointAsync(selectedLocalHost.LocalHost, ServiceNameForListener.Text);
                    rootPage.NotifyUser(
                        "Listening on address " + selectedLocalHost.LocalHost.CanonicalName, 
                        NotifyType.StatusMessage);
                }
                else if (BindToAdapter.IsChecked == true)
                {
                    // Try to limit traffic to the selected adapter. 
                    // This option will be overriden by interfaces with weak-host or forwarding modes enabled.
                    NetworkAdapter selectedAdapter = selectedLocalHost.LocalHost.IPInformation.NetworkAdapter;

                    await listener.BindServiceNameAsync(ServiceNameForListener.Text, selectedAdapter);

                    rootPage.NotifyUser(
                        "Listening on adapter " + selectedAdapter.NetworkAdapterId, 
                        NotifyType.StatusMessage);
                }
            }
            catch (Exception exception)
            {
                CoreApplication.Properties.Remove("listener");
                
                // If this is an unknown status it means that the error is fatal and retry will likely fail.
                if (SocketError.GetStatus(exception.HResult) == SocketErrorStatus.Unknown)
                {
                    throw;
                }

                rootPage.NotifyUser(
                    "Start listening failed with error: " + exception.Message, 
                    NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Message received handler
        /// </summary>
        /// <param name="socket">The socket object</param>
        /// <param name="eventArguments">The datagram event information</param>
        async void MessageReceived(DatagramSocket socket, DatagramSocketMessageReceivedEventArgs eventArguments)
        {
            object outObj;
            if (CoreApplication.Properties.TryGetValue("remotePeer", out outObj))
            {
                EchoMessage((RemotePeer)outObj, eventArguments);
                return;
            }

            // We do not have an output stream yet so create one.
            try
            {
                IOutputStream outputStream = await socket.GetOutputStreamAsync(
                    eventArguments.RemoteAddress, 
                    eventArguments.RemotePort);

                // It might happen that the OnMessage was invoked more than once before the GetOutputStreamAsync 
                // completed. In this case we will end up with multiple streams - make sure we have just one of it.
                RemotePeer peer;
                lock (this)
                {
                    if (CoreApplication.Properties.TryGetValue("remotePeer", out outObj))
                    {
                        peer = (RemotePeer)outObj;
                    }
                    else
                    {
                        peer = new RemotePeer(outputStream, eventArguments.RemoteAddress, eventArguments.RemotePort);
                        CoreApplication.Properties.Add("remotePeer", peer);
                    }
                }
                
                EchoMessage(peer, eventArguments);
            }
            catch (Exception exception)
            {
                // If this is an unknown status it means that the error is fatal and retry will likely fail.
                if (SocketError.GetStatus(exception.HResult) == SocketErrorStatus.Unknown)
                {
                    throw;
                }

                NotifyUserFromAsyncThread("Connect failed with error: " + exception.Message, NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Echo the message back to the peer
        /// </summary>
        /// <param name="peer">The remote peer object</param>
        /// <param name="eventArguments">The received message event arguments</param>
        async void EchoMessage(RemotePeer peer, DatagramSocketMessageReceivedEventArgs eventArguments)
        {
            if (!peer.IsMatching(eventArguments.RemoteAddress, eventArguments.RemotePort))
            {
                // In the sample we are communicating with just one peer. To communicate with multiple peers 
                // application should cache output streams (i.e. by using a hash map), because creating an output 
                // stream for each received datagram is costly. Keep in mind though, that every cache requires logic 
                // to remove old or unused elements; otherwise cache turns into a memory leaking structure.
                NotifyUserFromAsyncThread(
                    String.Format(
                        "Got datagram from {0}:{1}, but already 'connected' to {2}", 
                        eventArguments.RemoteAddress, 
                        eventArguments.RemotePort, 
                        peer), 
                    NotifyType.ErrorMessage);
            }

            try
            {
                await peer.OutputStream.WriteAsync(eventArguments.GetDataReader().DetachBuffer());
            }
            catch (Exception exception)
            {
                // If this is an unknown status it means that the error is fatal and retry will likely fail.
                if (SocketError.GetStatus(exception.HResult) == SocketErrorStatus.Unknown)
                {
                    throw;
                }

                NotifyUserFromAsyncThread("Send failed with error: " + exception.Message, NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Notifies the user from a non-UI thread
        /// </summary>
        /// <param name="strMessage">The message</param>
        /// <param name="type">The type of notification</param>
        private void NotifyUserFromAsyncThread(string strMessage, NotifyType type)
        {
            var ignore = Dispatcher.RunAsync(
                CoreDispatcherPriority.Normal, () => rootPage.NotifyUser(strMessage, type));
        }

        /// <summary>
        /// Populates the NetworkAdapter list
        /// </summary>
        private void PopulateAdapterList()
        {
            localHostItems.Clear();
            AdapterList.ItemsSource = localHostItems;
            AdapterList.DisplayMemberPath = "DisplayString";

            foreach (HostName localHostInfo in NetworkInformation.GetHostNames())
            {
                if (localHostInfo.IPInformation != null)
                {
                    LocalHostItem adapterItem = new LocalHostItem(localHostInfo);
                    localHostItems.Add(adapterItem);
                }
            }
        }
    }

    /// <summary>
    /// Helper class used to identify and respond to a remote peer
    /// </summary>
    class RemotePeer
    {
        IOutputStream outputStream;
        HostName hostName;
        String port;
        
        public RemotePeer(IOutputStream outputStream, HostName hostName, String port)
        {
            this.outputStream = outputStream;
            this.hostName = hostName;
            this.port = port;
        }

        public bool IsMatching(HostName hostName, String port)
        {
            return (this.hostName == hostName && this.port == port);
        }

        public IOutputStream OutputStream
        {
            get { return outputStream; }
        }

        public override String ToString()
        {
            return hostName + port;
        }
    }

    /// <summary>
    /// Helper class describing a NetworkAdapter and its associated IP address
    /// </summary>
    class LocalHostItem
    {
        public string DisplayString
        {
            get;
            private set;
        }

        public HostName LocalHost
        {
            get;
            private set;
        }

        public LocalHostItem(HostName localHostName)
        {
            if (localHostName == null)
            {
                throw new ArgumentNullException("localHostName");
            }

            if (localHostName.IPInformation == null)
            {
                throw new ArgumentException("Adapter information not found");
            }

            this.LocalHost = localHostName;
            this.DisplayString = "Address: " + localHostName.DisplayName + 
                " Adapter: " + localHostName.IPInformation.NetworkAdapter.NetworkAdapterId;
        }
    }
}
