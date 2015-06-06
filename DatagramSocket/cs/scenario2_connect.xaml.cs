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
using Windows.ApplicationModel.Core;
using Windows.Networking;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace DatagramSocketSample
{
    /// <summary>
    /// A page for second scenario.
    /// </summary>
    public sealed partial class Scenario2 : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario2()
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
            // Make sure we're using the correct server address if an adapter was selected in scenario 1
            object serverAddress;
            if (CoreApplication.Properties.TryGetValue("serverAddress", out serverAddress))
            {
                if (serverAddress is string)
                {
                    HostNameForConnect.Text = serverAddress as string;
                }
            }
        }

        /// <summary>
        /// This is the click handler for the 'ConnectSocket' button.
        /// </summary>
        /// <param name="sender">Object for which the event was generated.</param>
        /// <param name="e">Event's parameters.</param>
        private async void ConnectSocket_Click(object sender, RoutedEventArgs e)
        {
            if (String.IsNullOrEmpty(ServiceNameForConnect.Text))
            {
                rootPage.NotifyUser("Please provide a service name.", NotifyType.ErrorMessage);
                return;
            }

            // By default 'HostNameForConnect' is disabled and host name validation is not required. When enabling the
            // text box validating the host name is required since it was received from an untrusted source (user 
            // input). The host name is validated by catching ArgumentExceptions thrown by the HostName constructor for
            // invalid input.
            // Note that when enabling the text box users may provide names for hosts on the intErnet that require the
            // "Internet (Client)" capability.
            HostName hostName;
            try
            {
                hostName = new HostName(HostNameForConnect.Text);
            }
            catch (ArgumentException)
            {
                rootPage.NotifyUser("Error: Invalid host name.", NotifyType.ErrorMessage);
                return;
            }
            
            if (CoreApplication.Properties.ContainsKey("clientSocket"))
            {
                rootPage.NotifyUser(
                    "This step has already been executed. Please move to the next one.", 
                    NotifyType.ErrorMessage);
                return;
            }

            DatagramSocket socket = new DatagramSocket();

            if (DontFragment.IsOn)
            {
                // Set the IP DF (Don't Fragment) flag.
                // This won't have any effect when running both client and server on localhost.
                socket.Control.DontFragment = true;
            }

            socket.MessageReceived += MessageReceived;

            // Save the socket, so subsequent steps can use it.
            CoreApplication.Properties.Add("clientSocket", socket);

            rootPage.NotifyUser("Connecting to: " + HostNameForConnect.Text, NotifyType.StatusMessage);

            try
            {
                // Connect to the server (in our case the listener we created in previous step).
                await socket.ConnectAsync(hostName, ServiceNameForConnect.Text);

                rootPage.NotifyUser("Connected", NotifyType.StatusMessage);

                // Mark the socket as connected. Set the value to null, as we care only about the fact that the 
                // property is set.
                CoreApplication.Properties.Add("connected", null);
            }
            catch (Exception exception)
            {
                // If this is an unknown status it means that the error is fatal and retry will likely fail.
                if (SocketError.GetStatus(exception.HResult) == SocketErrorStatus.Unknown)
                {
                    throw;
                }

                rootPage.NotifyUser("Connect failed with error: " + exception.Message, NotifyType.ErrorMessage);
            }
        }

        void MessageReceived(DatagramSocket socket, DatagramSocketMessageReceivedEventArgs eventArguments)
        {
            try
            {
                uint stringLength = eventArguments.GetDataReader().UnconsumedBufferLength;
                NotifyUserFromAsyncThread(
                    "Receive data from remote peer: \"" + 
                    eventArguments.GetDataReader().ReadString(stringLength) + "\"", 
                    NotifyType.StatusMessage);
            }
            catch (Exception exception)
            {
                SocketErrorStatus socketError = SocketError.GetStatus(exception.HResult);
                if (socketError == SocketErrorStatus.ConnectionResetByPeer)
                {
                    // This error would indicate that a previous send operation resulted in an 
                    // ICMP "Port Unreachable" message.
                    NotifyUserFromAsyncThread(
                        "Peer does not listen on the specific port. Please make sure that you run step 1 first " +
                        "or you have a server properly working on a remote server.", 
                        NotifyType.ErrorMessage);
                }
                else if (socketError != SocketErrorStatus.Unknown)
                {
                    NotifyUserFromAsyncThread(
                        "Error happened when receiving a datagram: " + socketError.ToString(), 
                        NotifyType.ErrorMessage);
                }
                else
                {
                    throw;
                }
            }
        }

        private void NotifyUserFromAsyncThread(string strMessage, NotifyType type)
        {
            var ignore = Dispatcher.RunAsync(
                CoreDispatcherPriority.Normal, () => rootPage.NotifyUser(strMessage, type));
        }
    }
}
