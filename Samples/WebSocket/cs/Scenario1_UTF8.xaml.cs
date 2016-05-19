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
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
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
        private bool busy;

        public Scenario1()
        {
            this.InitializeComponent();
            UpdateVisualState();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            CloseSocket();    
        }

        private void UpdateVisualState()
        {
            if (busy)
            {
                VisualStateManager.GoToState(this, "Busy", false);
            }
            else
            {
                bool connected = (messageWebSocket != null);
                VisualStateManager.GoToState(this, connected ? "Connected" : "Disconnected", false);
            }
        }

        private void SetBusy(bool value)
        {
            busy = value;
            UpdateVisualState();
        }

        private async void OnConnect()
        {
            SetBusy(true);
            await ConnectAsync();
            SetBusy(false);
        }

        private async Task ConnectAsync()
        {
            if (String.IsNullOrEmpty(InputField.Text))
            {
                rootPage.NotifyUser("Please specify text to send", NotifyType.ErrorMessage);
                return;
            }

            // Validating the URI is required since it was received from an untrusted source (user input).
            // The URI is validated by calling TryGetUri() that will return 'false' for strings that are not
            // valid WebSocket URIs.
            // Note that when enabling the text box users may provide URIs to machines on the intrAnet
            // or intErnet. In these cases the app requires the "Home or Work Networking" or
            // "Internet (Client)" capability respectively.
            Uri server = rootPage.TryGetUri(ServerAddressField.Text);
            if (server == null)
            {
                return;
            }

            messageWebSocket = new MessageWebSocket();
            messageWebSocket.Control.MessageType = SocketMessageType.Utf8;
            messageWebSocket.MessageReceived += MessageReceived;
            messageWebSocket.Closed += OnClosed;

            AppendOutputLine($"Connecting to {server}...");
            try
            {
                await messageWebSocket.ConnectAsync(server);
            }
            catch (Exception ex) // For debugging
            {
                // Error happened during connect operation.
                messageWebSocket.Dispose();
                messageWebSocket = null;

                AppendOutputLine(MainPage.BuildWebSocketError(ex));
                AppendOutputLine(ex.Message);
                return;
            }

            // The default DataWriter encoding is Utf8.
            messageWriter = new DataWriter(messageWebSocket.OutputStream);
            rootPage.NotifyUser("Connected", NotifyType.StatusMessage);
        }

        async void OnSend()
        {
            SetBusy(true);
            await SendAsync();
            SetBusy(false);
        }

        async Task SendAsync()
        {
            string message = InputField.Text;
            if (String.IsNullOrEmpty(message))
            {
                rootPage.NotifyUser("Please specify text to send", NotifyType.ErrorMessage);
                return;
            }

            AppendOutputLine("Sending Message: " + message);

            // Buffer any data we want to send.
            messageWriter.WriteString(message);

            try
            {
                // Send the data as one complete message.
                await messageWriter.StoreAsync();
            }
            catch (Exception ex)
            {
                AppendOutputLine(MainPage.BuildWebSocketError(ex));
                AppendOutputLine(ex.Message);
                return;
            }

            rootPage.NotifyUser("Send Complete", NotifyType.StatusMessage);
        }

        private void MessageReceived(MessageWebSocket sender, MessageWebSocketMessageReceivedEventArgs args)
        {
            // Dispatch the event to the UI thread so we can update UI.
            var ignore = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                AppendOutputLine("Message Received; Type: " + args.MessageType);
                using (DataReader reader = args.GetDataReader())
                {
                    reader.UnicodeEncoding = UnicodeEncoding.Utf8;

                    try
                    {
                        string read = reader.ReadString(reader.UnconsumedBufferLength);
                        AppendOutputLine(read);
                    }
                    catch (Exception ex)
                    {
                        AppendOutputLine(MainPage.BuildWebSocketError(ex));
                        AppendOutputLine(ex.Message);
                    }
                }
            });
        }

        private void OnDisconnect()
        {
            SetBusy(true);
            rootPage.NotifyUser("Closing", NotifyType.StatusMessage);
            CloseSocket();
            SetBusy(false);
        }

        // This may be triggered remotely by the server or locally by Close/Dispose()
        private async void OnClosed(IWebSocket sender, WebSocketClosedEventArgs args)
        {
            // Dispatch the event to the UI thread so we do not need to synchronize access to messageWebSocket.
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                AppendOutputLine("Closed; Code: " + args.Code + ", Reason: " + args.Reason);

                if (messageWebSocket == sender)
                {
                    CloseSocket();
                    UpdateVisualState();
                }
            });
        }

        private void CloseSocket()
        {
            if (messageWebSocket != null)
            {
                try
                {
                    messageWebSocket.Close(1000, "Closed due to user request.");
                }
                catch (Exception ex)
                {
                    AppendOutputLine(MainPage.BuildWebSocketError(ex));
                    AppendOutputLine(ex.Message);
                }
                messageWebSocket = null;
            }
            if (messageWriter != null)
            {
                messageWriter.Dispose();
                messageWriter = null;
            }
        }

        private void AppendOutputLine(string value)
        {
            OutputField.Text += value + "\r\n";
        }
    }
}
