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
using Windows.Devices.Bluetooth.Rfcomm;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

namespace SDKTemplate
{
    public sealed partial class Scenario2_ChatServer : Page
    {
        private StreamSocket socket;
        private DataWriter writer;
        private RfcommServiceProvider rfcommProvider;
        private StreamSocketListener socketListener;

        // A pointer back to the main page is required to display status messages.
        MainPage rootPage = MainPage.Current;

        public Scenario2_ChatServer()
        {
            this.InitializeComponent();
        }

        private void ListenButton_Click(object sender, RoutedEventArgs e)
        {
            InitializeRfcommServer();
        }

        /// <summary>
        /// Initializes the server using RfcommServiceProvider to advertise the Chat Service UUID and start listening
        /// for incoming connections.
        /// </summary>
        private async void InitializeRfcommServer()
        {
            ListenButton.IsEnabled = false;
            DisconnectButton.IsEnabled = true;

            rfcommProvider = await RfcommServiceProvider.CreateAsync(
                RfcommServiceId.FromUuid(Constants.RfcommChatServiceUuid));

            // Create a listener for this service and start listening
            socketListener = new StreamSocketListener();
            socketListener.ConnectionReceived += OnConnectionReceived;

            await socketListener.BindServiceNameAsync(rfcommProvider.ServiceId.AsString(),
                SocketProtectionLevel.BluetoothEncryptionAllowNullAuthentication);

            // Set the SDP attributes and start Bluetooth advertising
            InitializeServiceSdpAttributes(rfcommProvider);
            rfcommProvider.StartAdvertising(socketListener);

            rootPage.NotifyUser("Listening for incoming connections", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Creates the SDP record that will be revealed to the Client device when pairing occurs.  
        /// </summary>
        /// <param name="rfcommProvider">The RfcommServiceProvider that is being used to initialize the server</param>
        private void InitializeServiceSdpAttributes(RfcommServiceProvider rfcommProvider)
        {
            var sdpWriter = new DataWriter();

            // Write the Service Name Attribute.
            sdpWriter.WriteByte(Constants.SdpServiceNameAttributeType);

            // The length of the UTF-8 encoded Service Name SDP Attribute.
            sdpWriter.WriteByte((byte)Constants.SdpServiceName.Length);

            // The UTF-8 encoded Service Name value.
            sdpWriter.UnicodeEncoding = Windows.Storage.Streams.UnicodeEncoding.Utf8;
            sdpWriter.WriteString(Constants.SdpServiceName);

            // Set the SDP Attribute on the RFCOMM Service Provider.
            rfcommProvider.SdpRawAttributes.Add(Constants.SdpServiceNameAttributeId, sdpWriter.DetachBuffer());
        }

        private void SendButton_Click(object sender, RoutedEventArgs e)
        {
            SendMessage();
        }

        public void KeyboardKey_Pressed(object sender, KeyRoutedEventArgs e)
        {
            if (e.Key == Windows.System.VirtualKey.Enter)
            {
                SendMessage();
            }
        }

        private async void SendMessage()
        {
            // There's no need to send a zero length message
            if (MessageTextBox.Text.Length != 0)
            {
                // Make sure that the connection is still up and there is a message to send
                if (socket != null)
                {
                    string message = MessageTextBox.Text;
                    writer.WriteUInt32((uint)message.Length);
                    writer.WriteString(message);

                    ConversationListBox.Items.Add("Sent: " + message);
                    // Clear the messageTextBox for a new message
                    MessageTextBox.Text = "";

                    await writer.StoreAsync();

                }
                else
                {
                    rootPage.NotifyUser("No clients connected, please wait for a client to connect before attempting to send a message", NotifyType.StatusMessage);
                }
            }
        }


        private void DisconnectButton_Click(object sender, RoutedEventArgs e)
        {
            Disconnect();
            rootPage.NotifyUser("Disconnected.", NotifyType.StatusMessage);
        }

        private async void Disconnect()
        {
            if (rfcommProvider != null)
            {
                rfcommProvider.StopAdvertising();
                rfcommProvider = null;
            }

            if (socketListener != null)
            {
                socketListener.Dispose();
                socketListener = null;
            }

            if (writer != null)
            {
                writer.DetachStream();
                writer = null;
            }

            if (socket != null)
            {
                socket.Dispose();
                socket = null;
            }
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                ListenButton.IsEnabled = true;
                DisconnectButton.IsEnabled = false;
                ConversationListBox.Items.Clear();
            });
        }

        /// <summary>
        /// Invoked when the socket listener accepts an incoming Bluetooth connection.
        /// </summary>
        /// <param name="sender">The socket listener that accepted the connection.</param>
        /// <param name="args">The connection accept parameters, which contain the connected socket.</param>
        private async void OnConnectionReceived(
            StreamSocketListener sender, StreamSocketListenerConnectionReceivedEventArgs args)
        {
            // Don't need the listener anymore
            socketListener.Dispose();
            socketListener = null;

            socket = args.Socket;

            writer = new DataWriter(socket.OutputStream);

            var reader = new DataReader(socket.InputStream);
            bool remoteDisconnection = false;

            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                rootPage.NotifyUser("Client Connected", NotifyType.StatusMessage);
            });            

            // Infinite read buffer loop
            while (true)
            {
                try
                {
                    // Based on the protocol we've defined, the first uint is the size of the message
                    uint readLength = await reader.LoadAsync(sizeof(uint));

                    // Check if the size of the data is expected (otherwise the remote has already terminated the connection)
                    if (readLength < sizeof(uint))
                    {
                        remoteDisconnection = true;
                        break;
                    }
                    uint currentLength = reader.ReadUInt32();

                    // Load the rest of the message since you already know the length of the data expected.  
                    readLength = await reader.LoadAsync(currentLength);

                    // Check if the size of the data is expected (otherwise the remote has already terminated the connection)
                    if (readLength < currentLength)
                    {
                        remoteDisconnection = true;
                        break;
                    }
                    string message = reader.ReadString(currentLength);

                    await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
                    {
                        ConversationListBox.Items.Add("Received: " + message);
                    });
                }
                catch(Exception ex)
                {
                    switch ((uint)ex.HResult)
                    {
                        case (0x800703E3):
                            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
                            {
                                rootPage.NotifyUser("Client Disconnected Successfully", NotifyType.StatusMessage);
                            });
                            break;
                        default:
                            throw;
                    }
                    break;

                }
            }

            reader.DetachStream();
            if (remoteDisconnection)
            {
                Disconnect();
                await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
                {
                    rootPage.NotifyUser("Client disconnected.",NotifyType.StatusMessage);
                });
            }
        }
    }
}
