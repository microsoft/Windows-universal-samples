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

            // If we are connecting to wss:// endpoint, by default, the OS performs validation of
            // the server certificate based on well-known trusted CAs. We can perform additional custom
            // validation if needed.
            if (SecureWebSocketCheckBox.IsChecked == true)
            {
                // WARNING: Only test applications should ignore SSL errors.
                // In real applications, ignoring server certificate errors can lead to Man-In-The-Middle
                // attacks. (Although the connection is secure, the server is not authenticated.)
                // Note that not all certificate validation errors can be ignored.
                // In this case, we are ignoring these errors since the certificate assigned to the localhost
                // URI is self-signed and has subject name = fabrikam.com
                messageWebSocket.Control.IgnorableServerCertificateErrors.Add(ChainValidationResult.Untrusted);
                messageWebSocket.Control.IgnorableServerCertificateErrors.Add(ChainValidationResult.InvalidName);

                // Add event handler to listen to the ServerCustomValidationRequested event. This enables performing
                // custom validation of the server certificate. The event handler must implement the desired
                // custom certificate validation logic.
                messageWebSocket.ServerCustomValidationRequested += OnServerCustomValidationRequested;

                // Certificate validation occurs only for secure connections.
                if (server.Scheme != "wss")
                {
                    AppendOutputLine("Note: Certificate validation is performed only for the wss: scheme.");
                }
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

            // The default DataWriter encoding is Utf8.
            messageWriter = new DataWriter(messageWebSocket.OutputStream);
            rootPage.NotifyUser("Connected", NotifyType.StatusMessage);
        }

        private async void OnServerCustomValidationRequested(MessageWebSocket sender, WebSocketServerCustomValidationRequestedEventArgs args)
        {
            // In order to call async APIs in this handler, you must first take a deferral and then
            // release it once you are done with the operation. The "using" statement
            // ensures that the deferral completes when control leaves the block.
            bool isValid;
            using (Deferral deferral = args.GetDeferral())
            {
                // Get the server certificate and certificate chain from the args parameter.
                isValid = await MainPage.AreCertificateAndCertChainValidAsync(args.ServerCertificate, args.ServerIntermediateCertificates);

                if (!isValid)
                {
                    args.Reject();
                }
            }

            // Continue on the UI thread so we can update UI.
            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                if (isValid)
                {
                    AppendOutputLine("Custom validation of server certificate passed.");
                }
                else
                {
                    AppendOutputLine("Custom validation of server certificate failed.");
                }
            });
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
            if (messageWriter != null)
            {
                // In order to reuse the socket with another DataWriter, the socket's output stream needs to be detached.
                // Otherwise, the DataWriter's destructor will automatically close the stream and all subsequent I/O operations
                // invoked on the socket's output stream will fail with ObjectDisposedException.
                //
                // This is only added for completeness, as this sample closes the socket in the very next code block.
                messageWriter.DetachStream();
                messageWriter.Dispose();
                messageWriter = null;
            }

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
