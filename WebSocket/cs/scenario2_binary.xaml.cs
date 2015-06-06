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
using System.IO;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading;
using System.Threading.Tasks;
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
    public sealed partial class Scenario2 : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private StreamWebSocket streamWebSocket;
        private byte[] readBuffer;

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
        }

        private async void Start_Click(object sender, RoutedEventArgs e)
        {
            // Have we connected yet?
            if (streamWebSocket != null)
            {
                rootPage.NotifyUser("Already connected", NotifyType.StatusMessage);
                return;
            }

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

            try
            {
                rootPage.NotifyUser("Connecting to: " + server, NotifyType.StatusMessage);

                streamWebSocket = new StreamWebSocket();

                // Dispatch close event on UI thread. This allows us to avoid synchronizing access to streamWebSocket.
                streamWebSocket.Closed += async (senderSocket, args) =>
                {
                    await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => Closed(senderSocket, args));
                };

                await streamWebSocket.ConnectAsync(server);

                readBuffer = new byte[1000];

                // Start a background task to continuously read for incoming data
                Task receiving = Task.Factory.StartNew(Scenario2ReceiveData,
                    streamWebSocket.InputStream.AsStreamForRead(), TaskCreationOptions.LongRunning);

                // Start a background task to continuously write outgoing data
                Task sending = Task.Factory.StartNew(Scenario2SendData,
                    streamWebSocket.OutputStream, TaskCreationOptions.LongRunning);

                rootPage.NotifyUser("Connected", NotifyType.StatusMessage);
            }
            catch (Exception ex) // For debugging
            {
                if (streamWebSocket != null)
                {
                    streamWebSocket.Dispose();
                    streamWebSocket = null;
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

        // Continuously write outgoing data. For writing data we'll show how to use data.AsBuffer() to get an
        // IBuffer for use with webSocket.OutputStream.WriteAsync.  Alternatively you can call
        // webSocket.OutputStream.AsStreamForWrite() to use .NET streams.
        private async void Scenario2SendData(object state)
        {
            int dataSent = 0;
            byte[] data = new byte[] { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };

            MarshalText(OutputField, "Background sending data in " + data.Length
                + " byte chunks each second.\r\n");

            try
            {
                IOutputStream writeStream = (IOutputStream)state;

                // Send until the socket gets closed/stopped
                while (true)
                {
                    // using System.Runtime.InteropServices.WindowsRuntime;
                    await writeStream.WriteAsync(data.AsBuffer());

                    dataSent += data.Length;
                    MarshalText(DataSentField, dataSent.ToString(), false);

                    // Delay so the user can watch what's going on.
                    await Task.Delay(TimeSpan.FromSeconds(1));
                }
            }
            catch (ObjectDisposedException)
            {
                MarshalText(OutputField, "Background write stopped.\r\n");
            }
            catch (Exception ex)
            {
                WebErrorStatus status = WebSocketError.GetStatus(ex.GetBaseException().HResult);

                switch (status)
                {
                    case WebErrorStatus.OperationCanceled:
                        MarshalText(OutputField, "Background write canceled.\r\n");
                        break;

                    case WebErrorStatus.Unknown:
                        throw;

                    default:
                        MarshalText(OutputField, "Error: " + status + "\r\n");
                        MarshalText(OutputField, ex.Message + "\r\n");
                        break;
                }
            }
        }

        // Continuously read incoming data. For reading data we'll show how to use webSocket.InputStream.AsStream()
        // to get a .NET stream. Alternatively you could call readBuffer.AsBuffer() to use IBuffer with
        // webSocket.InputStream.ReadAsync.
        private async void Scenario2ReceiveData(object state)
        {
            int bytesReceived = 0;
            try
            {
                Stream readStream = (Stream)state;
                MarshalText(OutputField, "Background read starting.\r\n");

                while (true) // Until closed and ReadAsync fails.
                {
                    int read = await readStream.ReadAsync(readBuffer, 0, readBuffer.Length);
                    bytesReceived += read;
                    MarshalText(DataReceivedField, bytesReceived.ToString(), false);

                    // Do something with the data.
                }
            }
            catch (ObjectDisposedException)
            {
                MarshalText(OutputField, "Background read stopped.\r\n");
            }
            catch (Exception ex)
            {
                WebErrorStatus status = WebSocketError.GetStatus(ex.GetBaseException().HResult);

                switch (status)
                {
                    case WebErrorStatus.OperationCanceled:
                        MarshalText(OutputField, "Background write canceled.\r\n");
                        break;

                    case WebErrorStatus.Unknown:
                        throw;

                    default:
                        MarshalText(OutputField, "Error: " + status + "\r\n");
                        MarshalText(OutputField, ex.Message + "\r\n");
                        break;
                }
            }
        }

        private void Stop_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (streamWebSocket != null)
                {
                    rootPage.NotifyUser("Stopping", NotifyType.StatusMessage);
                    streamWebSocket.Close(1000, "Closed due to user request.");
                    streamWebSocket = null;
                }
                else
                {
                    rootPage.NotifyUser("There is no active socket to stop.", NotifyType.StatusMessage);
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

            if (streamWebSocket != null)
            {
                streamWebSocket.Dispose();
                streamWebSocket = null;
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
