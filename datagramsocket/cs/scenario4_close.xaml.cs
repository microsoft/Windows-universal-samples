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
using System.Diagnostics;
using Windows.ApplicationModel.Core;
using Windows.Networking;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace DatagramSocketSample
{
    /// <summary>
    /// A page for fourth scenario.
    /// </summary>
    public sealed partial class Scenario4 : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario4()
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
        }

        /// <summary>
        /// This is the click handler for the 'CloseSockets' button.
        /// </summary>
        /// <param name="sender">Object for which the event was generated.</param>
        /// <param name="e">Event's parameters.</param>
        private void CloseSockets_Click(object sender, RoutedEventArgs e)
        {
            object outValue;
            if (CoreApplication.Properties.TryGetValue("clientDataWriter", out outValue))
            {
                // Remove the data writer from the list of application properties as we are about to close it.
                CoreApplication.Properties.Remove("clientDataWriter");
                DataWriter dataWriter = (DataWriter)outValue;

                // To reuse the socket with other data writer, application has to detach the stream from the writer
                // before disposing it. This is added for completeness, as this sample closes the socket in
                // very next block.
                dataWriter.DetachStream();
                dataWriter.Dispose();
            }

            if (CoreApplication.Properties.TryGetValue("clientSocket", out outValue))
            {
                // Remove the socket from the list of application properties as we are about to close it.
                CoreApplication.Properties.Remove("clientSocket");
                DatagramSocket socket = (DatagramSocket)outValue;
                socket.Dispose();
            }

            if (CoreApplication.Properties.TryGetValue("listener", out outValue))
            {
                // Remove the listener from the list of application properties as we are about to close it.
                CoreApplication.Properties.Remove("listener");
                DatagramSocket listener = (DatagramSocket)outValue;
                listener.Dispose();
            }

            CoreApplication.Properties.Remove("remotePeer");
            CoreApplication.Properties.Remove("connected");
            CoreApplication.Properties.Remove("serverAddress");

            rootPage.NotifyUser("Socket and listener closed", NotifyType.StatusMessage);
        }

    }
}
