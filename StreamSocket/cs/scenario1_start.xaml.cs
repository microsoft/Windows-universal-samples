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

namespace StreamSocketSample
{
    /// <summary>
    /// A page for first scenario.
    /// </summary>
    public sealed partial class Scenario1 : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        // List containing all available local HostName endpoints
        private List<LocalHostItem> localHostItems = new List<LocalHostItem>();

        public Scenario1()
        {
            this.InitializeComponent();
            PopulateAdapterList();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            BindToAny.IsChecked = true;
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
            // Overriding the listener here is safe as it will be deleted once all references to it are gone. 
            // However, in many cases this is a dangerous pattern to override data semi-randomly (each time user 
            // clicked the button) so we block it here.
            if (CoreApplication.Properties.ContainsKey("listener"))
            {
                rootPage.NotifyUser(
                    "This step has already been executed. Please move to the next one.", 
                    NotifyType.ErrorMessage);
                return;
            }

            if (String.IsNullOrEmpty(ServiceNameForListener.Text))
            {
                rootPage.NotifyUser("Please provide a service name.", NotifyType.ErrorMessage);
                return;
            }

            CoreApplication.Properties.Remove("serverAddress");
            CoreApplication.Properties.Remove("adapter");
            
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

            StreamSocketListener listener = new StreamSocketListener();
            listener.ConnectionReceived += OnConnection;
            
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

                    // For demo purposes, ensure that use the same adapter in the client connect scenario.
                    CoreApplication.Properties.Add("adapter", selectedAdapter);

                    await listener.BindServiceNameAsync(
                        ServiceNameForListener.Text, 
                        SocketProtectionLevel.PlainSocket,
                        selectedAdapter);

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
        /// Invoked once a connection is accepted by StreamSocketListener.
        /// </summary>
        /// <param name="sender">The listener that accepted the connection.</param>
        /// <param name="args">Parameters associated with the accepted connection.</param>
        private async void OnConnection(
            StreamSocketListener sender, 
            StreamSocketListenerConnectionReceivedEventArgs args)
        {
            DataReader reader = new DataReader(args.Socket.InputStream);
            try
            {
                while (true)
                {
                    // Read first 4 bytes (length of the subsequent string).
                    uint sizeFieldCount = await reader.LoadAsync(sizeof(uint));
                    if (sizeFieldCount != sizeof(uint))
                    {
                        // The underlying socket was closed before we were able to read the whole data.
                        return;
                    }

                    // Read the string.
                    uint stringLength = reader.ReadUInt32();
                    uint actualStringLength = await reader.LoadAsync(stringLength);
                    if (stringLength != actualStringLength)
                    {
                        // The underlying socket was closed before we were able to read the whole data.
                        return;
                    }

                    // Display the string on the screen. The event is invoked on a non-UI thread, so we need to marshal
                    // the text back to the UI thread.
                    NotifyUserFromAsyncThread(
                        String.Format("Received data: \"{0}\"", reader.ReadString(actualStringLength)), 
                        NotifyType.StatusMessage);
                }
            }
            catch (Exception exception)
            {
                // If this is an unknown status it means that the error is fatal and retry will likely fail.
                if (SocketError.GetStatus(exception.HResult) == SocketErrorStatus.Unknown)
                {
                    throw;
                }

                NotifyUserFromAsyncThread(
                    "Read stream failed with error: " + exception.Message, 
                    NotifyType.ErrorMessage);
            }
        }

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
