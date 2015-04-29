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
using Windows.Networking.Sockets;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Web;

namespace Microsoft.Samples.Networking.WebSocket
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1 : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private MessageWebSocket messageWebSocket;
        private DataWriter messageWriter;

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
        }

        private async void Start_Click(object sender, RoutedEventArgs e)
        {
            if (String.IsNullOrEmpty(InputField.Text))
            {
                rootPage.NotifyUser("Please specify text to send", NotifyType.ErrorMessage);
                return;
            }

            bool connecting = true;
            try
            {
                // Have we connected yet?
                if (messageWebSocket == null)
                {
                    // Validating the URI is required since it was received from an untrusted source (user input). 
                    // The URI is validated by calling TryGetUri() that will return 'false' for strings that are not
                    // valid WebSocket URIs.
                    // Note that when enabling the text box users may provide URIs to machines on the intrAnet 
                    // or intErnet. In these cases the app requires the "Home or Work Networking" or 
                    // "Internet (Client)" capability respectively.
                    Uri server;
                    if (!rootPage.TryGetUri(ServerAddressField.Text, out server))
                    {
                        return;
                    }

                    rootPage.NotifyUser("Connecting to: " + server, NotifyType.StatusMessage);

                    messageWebSocket = new MessageWebSocket();
                    messageWebSocket.Control.MessageType = SocketMessageType.Utf8;
                    messageWebSocket.MessageReceived += MessageReceived;

                    // Dispatch close event on UI thread. This allows us to avoid synchronizing access to messageWebSocket.
                    messageWebSocket.Closed += async (senderSocket, args) =>
                    {
                        await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => Closed(senderSocket, args));
                    };

                    await messageWebSocket.ConnectAsync(server);
                    messageWriter = new DataWriter(messageWebSocket.OutputStream);

                    rootPage.NotifyUser("Connected", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("Already connected", NotifyType.StatusMessage);
                }

                connecting = false;
                string message = InputField.Text;
                OutputField.Text += "Sending Message:\r\n" + message + "\r\n";

                // Buffer any data we want to send.
                messageWriter.WriteString(message);

                // Send the data as one complete message.
                await messageWriter.StoreAsync();

                rootPage.NotifyUser("Send Complete", NotifyType.StatusMessage);
            }
            catch (Exception ex) // For debugging
            {
                // Error happened during connect operation.
                if (connecting && messageWebSocket != null)
                {
                    messageWebSocket.Dispose();
                    messageWebSocket = null;
                }

                WebErrorStatus status = WebSocketError.GetStatus(ex.GetBaseException().HResult);

                switch (status)
                {
                    case WebErrorStatus.CannotConnect:
                    case WebErrorStatus.NotFound:
                    case WebErrorStatus.RequestTimeout:
                        rootPage.NotifyUser("Cannot connect to the server. Please make sure " +
                            "to run the server setup script before running the sample.", NotifyType.ErrorMessage);
                        break;

                    case WebErrorStatus.Unknown:
                        throw;

                    default:
                        rootPage.NotifyUser("Error: " + status, NotifyType.ErrorMessage);
                        break;
                }

                OutputField.Text += ex.Message + "\r\n";
            }
        }

        private void MessageReceived(MessageWebSocket sender, MessageWebSocketMessageReceivedEventArgs args)
        {
            try
            {
                MarshalText(OutputField, "Message Received; Type: " + args.MessageType + "\r\n");
                using (DataReader reader = args.GetDataReader())
                {
                    reader.UnicodeEncoding = Windows.Storage.Streams.UnicodeEncoding.Utf8;

                    string read = reader.ReadString(reader.UnconsumedBufferLength);
                    MarshalText(OutputField, read + "\r\n");
                }
            }
            catch (Exception ex) // For debugging
            {
                WebErrorStatus status = WebSocketError.GetStatus(ex.GetBaseException().HResult);

                if (status == WebErrorStatus.Unknown)
                {
                    throw;
                }

                // Normally we'd use the status to test for specific conditions we want to handle specially,
                // and only use ex.Message for display purposes.  In this sample, we'll just output the
                // status for debugging here, but still use ex.Message below.
                MarshalText(OutputField, "Error: " + status + "\r\n");

                MarshalText(OutputField, ex.Message + "\r\n");
            }
        }

        private void Close_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (messageWebSocket != null)
                {
                    rootPage.NotifyUser("Closing", NotifyType.StatusMessage);
                    messageWebSocket.Close(1000, "Closed due to user request.");
                    messageWebSocket = null;
                }
                else
                {
                    rootPage.NotifyUser("No active WebSocket, send something first", NotifyType.StatusMessage);
                }
            }
            catch (Exception ex)
            {
                WebErrorStatus status = WebSocketError.GetStatus(ex.GetBaseException().HResult);

                if (status == WebErrorStatus.Unknown)
                {
                    throw;
                }

                // Normally we'd use the status to test for specific conditions we want to handle specially,
                // and only use ex.Message for display purposes.  In this sample, we'll just output the
                // status for debugging here, but still use ex.Message below.
                rootPage.NotifyUser("Error: " + status, NotifyType.ErrorMessage);

                OutputField.Text += ex.Message + "\r\n";
            }
        }

        // This may be triggered remotely by the server or locally by Close/Dispose()
        private void Closed(IWebSocket sender, WebSocketClosedEventArgs args)
        {
            MarshalText(OutputField, "Closed; Code: " + args.Code + ", Reason: " + args.Reason + "\r\n");

            if (messageWebSocket != null)
            {
                messageWebSocket.Dispose();
                messageWebSocket = null;
            }
        }

        private void MarshalText(TextBox output, string value)
        {
            MarshalText(output, value, true);
        }

        // When operations happen on a background thread we have to marshal UI updates back to the UI thread.
        private void MarshalText(TextBox output, string value, bool append)
        {
            var ignore = output.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                if (append)
                {
                    output.Text += value;
                }
                else
                {
                    output.Text = value;
                }
            });
        }
    }
}
