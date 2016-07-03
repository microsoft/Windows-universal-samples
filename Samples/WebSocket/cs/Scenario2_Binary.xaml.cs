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
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Networking.Sockets;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Web;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2 : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private StreamWebSocket streamWebSocket;
        private bool busy;

        public Scenario2()
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
                bool connected = (streamWebSocket != null);
                VisualStateManager.GoToState(this, connected ? "Connected" : "Disconnected", false);
            }
        }

        private void SetBusy(bool value)
        {
            busy = value;
            UpdateVisualState();
        }

        private async void OnStart()
        {
            SetBusy(true);
            await StartAsync();
            SetBusy(false);
        }

        private async Task StartAsync()
        {
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

            streamWebSocket = new StreamWebSocket();
            streamWebSocket.Closed += OnClosed;

            AppendOutputLine($"Connecting to {server}...");
            try
            {
                await streamWebSocket.ConnectAsync(server);
            }
            catch (Exception ex) // For debugging
            {
                streamWebSocket.Dispose();
                streamWebSocket = null;

                AppendOutputLine(MainPage.BuildWebSocketError(ex));
                AppendOutputLine(ex.Message);
                return;
            }
            rootPage.NotifyUser("Connected", NotifyType.StatusMessage);

            // Start a task to continuously read for incoming data
            Task receiving = ReceiveDataAsync(streamWebSocket);

            // Start a task to continuously write outgoing data
            Task sending = SendDataAsync(streamWebSocket);
        }

        // Continuously write outgoing data. For writing data we'll show how to use data.AsBuffer() to get an
        // IBuffer for use with activeSocket.OutputStream.WriteAsync.  Alternatively you can call
        // activeSocket.OutputStream.AsStreamForWrite() to use .NET streams.
        private async Task SendDataAsync(StreamWebSocket activeSocket)
        {
            int bytesSent = 0;
            byte[] data = new byte[] { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };

            AppendOutputLine($"Background sending data in {data.Length} byte chunks each second.");

            try
            {
                // Send until the socket gets closed/stopped
                while (true)
                {
                    if (streamWebSocket != activeSocket)
                    {
                        // Our socket is no longer active. Stop sending.
                        AppendOutputLine("Background write stopped.");
                        return;
                    }

                    await activeSocket.OutputStream.WriteAsync(data.AsBuffer());

                    bytesSent += data.Length;
                    DataSentField.Text = bytesSent.ToString();

                    // Delay so the user can watch what's going on.
                    await Task.Delay(TimeSpan.FromSeconds(1));
                }
            }
            catch (Exception ex)
            {
                WebErrorStatus status = WebSocketError.GetStatus(ex.GetBaseException().HResult);

                switch (status)
                {
                    case WebErrorStatus.OperationCanceled:
                        AppendOutputLine("Background write canceled.");
                        break;

                    default:
                        AppendOutputLine("Error: " + status);
                        AppendOutputLine(ex.Message);
                        break;
                }
            }
        }

        // Continuously read incoming data. For reading data we'll show how to use activeSocket.InputStream.AsStream()
        // to get a .NET stream. Alternatively you could call readBuffer.AsBuffer() to use IBuffer with
        // activeSocket.InputStream.ReadAsync.
        private async Task ReceiveDataAsync(StreamWebSocket activeSocket)
        {
            Stream readStream = streamWebSocket.InputStream.AsStreamForRead();
            int bytesReceived = 0;
            try
            {
                AppendOutputLine("Background read starting.");

                byte[] readBuffer = new byte[1000];

                while (true)
                {
                    if (streamWebSocket != activeSocket)
                    {
                        // Our socket is no longer active. Stop reading.
                        AppendOutputLine("Background read stopped.");
                        return;
                    }

                    int read = await readStream.ReadAsync(readBuffer, 0, readBuffer.Length);

                    // Do something with the data.
                    // This sample merely reports that the data was received.

                    bytesReceived += read;
                    DataReceivedField.Text = bytesReceived.ToString();
                }
            }
            catch (Exception ex)
            {
                WebErrorStatus status = WebSocketError.GetStatus(ex.GetBaseException().HResult);

                switch (status)
                {
                    case WebErrorStatus.OperationCanceled:
                        AppendOutputLine("Background read canceled.");
                        break;

                    default:
                        AppendOutputLine("Error: " + status);
                        AppendOutputLine(ex.Message);
                        break;
                }
            }
        }

        private void OnStop()
        {
            SetBusy(true);
            rootPage.NotifyUser("Stopping", NotifyType.StatusMessage);
            CloseSocket();
            SetBusy(false);
        }

        // This may be triggered remotely by the server or locally by Close/Dispose()
        private async void OnClosed(IWebSocket sender, WebSocketClosedEventArgs args)
        {
            // Dispatch the event to the UI thread so we do not need to synchronize access to streamWebSocket.
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                AppendOutputLine("Closed; Code: " + args.Code + ", Reason: " + args.Reason);

                if (streamWebSocket == sender)
                {
                    CloseSocket();
                    UpdateVisualState();
                }
            });
        }

        private void CloseSocket()
        {
            if (streamWebSocket != null)
            {
                try
                {
                    streamWebSocket.Close(1000, "Closed due to user request.");
                }
                catch (Exception ex)
                {
                    AppendOutputLine(MainPage.BuildWebSocketError(ex));
                    AppendOutputLine(ex.Message);
                }
                streamWebSocket = null;
            }
        }

        private void AppendOutputLine(string value)
        {
            OutputField.Text += value + "\r\n";
        }
    }
}
