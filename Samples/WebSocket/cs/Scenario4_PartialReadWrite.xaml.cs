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
using System.IO;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Networking.Sockets;
using Windows.Security.Cryptography.Certificates;
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
    public sealed partial class Scenario4 : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private MessageWebSocket messageWebSocket;
        private bool busy;

        public Scenario4()
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
            if (String.IsNullOrEmpty(ServerAddressField.Text))
            {
                rootPage.NotifyUser("Please specify a URI to connect to", NotifyType.ErrorMessage);
                return;
            }

            // Validating the URI is required since it was received from an untrusted source (user input).
            // The URI is validated by calling TryGetUri() that will return 'false' for strings that are not
            // valid WebSocket URIs.
            // Note that when enabling the text box users may provide URIs to machines on the intrAnet
            // or intErnet. In these cases the app requires the "Private Networks (Client and Server)" or
            // "Internet (Client)" capability respectively.
            Uri server = rootPage.TryGetUri(ServerAddressField.Text);
            if (server == null)
            {
                return;
            }

            messageWebSocket = new MessageWebSocket();
            messageWebSocket.Control.MessageType = SocketMessageType.Utf8;

            // To support receiving event notifications for partial messages, you must set this receive mode.
            // If you do not set this mode, you will only receive notifications for complete messages; which is
            // the default behavior. Setting to Partial allows us to process partial data as soon as it arrives,
            // as opposed to waiting until the EndOfMessage to process the entire data.
            messageWebSocket.Control.ReceiveMode = MessageWebSocketReceiveMode.PartialMessage;

            messageWebSocket.MessageReceived += MessageReceived;
            messageWebSocket.Closed += OnClosed;

            if (server.Scheme == "wss")
            {
                // WARNING: Only test applications should ignore SSL errors.
                // In real applications, ignoring server certificate errors can lead to Man-In-The-Middle
                // attacks. (Although the connection is secure, the server is not authenticated.)
                // Note that not all certificate validation errors can be ignored.
                // In this case, we are ignoring these errors since the certificate assigned to the localhost
                // URI is self-signed and has subject name = fabrikam.com
                messageWebSocket.Control.IgnorableServerCertificateErrors.Add(ChainValidationResult.Untrusted);
                messageWebSocket.Control.IgnorableServerCertificateErrors.Add(ChainValidationResult.InvalidName);
            }

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
            
            try
            {
                DataWriter messageWriter = new DataWriter();
                messageWriter.WriteString(message);
                IBuffer buffer = messageWriter.DetachBuffer();

                if (EndOfMessageCheckBox.IsChecked == true)
                {
                    AppendOutputLine("Sending end of message: " + message);
                    await messageWebSocket.SendFinalFrameAsync(buffer);
                }
                else
                {
                    AppendOutputLine("Sending partial message: " + message);
                    await messageWebSocket.SendNonfinalFrameAsync(buffer);
                }
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
                string partialOrCompleted = "Partial";

                if (args.IsMessageComplete)
                {
                    partialOrCompleted = "Complete";
                }

                AppendOutputLine(partialOrCompleted + " message received; Type: " + args.MessageType);

                using (DataReader reader = args.GetDataReader())
                {
                    reader.UnicodeEncoding = UnicodeEncoding.Utf8;

                    try
                    {
                        // Note that it may be possible for partial UTF8 messages to be split between a character if it
                        // extends multiple bytes. We avoid this by using only ASCII characters in this example. 
                        // Should your application use multi-byte characters, you will need to do checks for broken characters.
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
            rootPage.NotifyUser("Closed", NotifyType.StatusMessage);
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
        }

        private void AppendOutputLine(string value)
        {
            OutputField.Text += value + "\r\n";
        }
    }
}
