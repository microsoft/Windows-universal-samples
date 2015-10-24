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
using System.Collections.Generic;
using Windows.Devices.Bluetooth.Rfcomm;
using Windows.Devices.Enumeration;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_ChatClient : Page
    {
        private StreamSocket chatSocket = null;
        private DataWriter chatWriter = null;
        private RfcommDeviceService chatService = null;
        private DeviceInformationCollection chatServiceDeviceCollection = null;

        // A pointer back to the main page is required to display status messages.
        private MainPage rootPage = MainPage.Current;

        public Scenario1_ChatClient()
        {
            this.InitializeComponent();
            App.Current.Suspending += App_Suspending;
        }

        void App_Suspending(object sender, Windows.ApplicationModel.SuspendingEventArgs e)
        {
            // Make sure we clean up resources on suspend.
            Disconnect("App Suspension disconnects");
        }

        /// <summary>
        /// When the user presses the run button, check to see if any of the currently paired devices support the Rfcomm chat service and display them in a list.  
        /// Note that in this case, the other device must be running the Rfcomm Chat Server before being paired.  
        /// </summary>
        /// <param name="sender">Instance that triggered the event.</param>
        /// <param name="e">Event data describing the conditions that led to the event.</param>
        private async void RunButton_Click(object sender, RoutedEventArgs e)
        {
            var button = sender as Button;
            // Disable the button while we do async operations so the user can't Run twice.
            button.IsEnabled = false;

            // Clear any previous messages
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            // Find all paired instances of the Rfcomm chat service and display them in a list
            chatServiceDeviceCollection = await DeviceInformation.FindAllAsync(
                RfcommDeviceService.GetDeviceSelector(RfcommServiceId.FromUuid(Constants.RfcommChatServiceUuid)));

            if (chatServiceDeviceCollection.Count > 0)
            {
                DeviceList.Items.Clear();
                foreach (var chatServiceDevice in chatServiceDeviceCollection)
                {
                    DeviceList.Items.Add(chatServiceDevice.Name);
                }
                DeviceList.Visibility = Windows.UI.Xaml.Visibility.Visible;
            }
            else
            {
                rootPage.NotifyUser(
                    "No chat services were found. Please pair with a device that is advertising the chat service.",
                    NotifyType.ErrorMessage);
            }
            button.IsEnabled = true;
        }

        /// <summary>
        /// Invoked once the user has selected the device to connect to.  
        /// Once the user has selected the device, 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void DeviceList_Tapped(object sender, TappedRoutedEventArgs e)
        {   
            RunButton.IsEnabled = false;
            DeviceList.Visibility = Windows.UI.Xaml.Visibility.Collapsed;

            var chatServiceDevice = chatServiceDeviceCollection[DeviceList.SelectedIndex];
            chatService = await RfcommDeviceService.FromIdAsync(chatServiceDevice.Id);
                
            if (chatService == null)
            {
                rootPage.NotifyUser(
                    "Access to the device is denied because the application was not granted access",
                    NotifyType.StatusMessage);
                return;
            }

            // Do various checks of the SDP record to make sure you are talking to a device that actually supports the Bluetooth Rfcomm Chat Service 
            var attributes = await chatService.GetSdpRawAttributesAsync();
            if (!attributes.ContainsKey(Constants.SdpServiceNameAttributeId))
            {
                rootPage.NotifyUser(
                    "The Chat service is not advertising the Service Name attribute (attribute id=0x100). " +
                    "Please verify that you are running the BluetoothRfcommChat server.",
                    NotifyType.ErrorMessage);
                RunButton.IsEnabled = true;
                return;
            }

            var attributeReader = DataReader.FromBuffer(attributes[Constants.SdpServiceNameAttributeId]);
            var attributeType = attributeReader.ReadByte();
            if (attributeType != Constants.SdpServiceNameAttributeType)
            {
                rootPage.NotifyUser(
                    "The Chat service is using an unexpected format for the Service Name attribute. " +
                    "Please verify that you are running the BluetoothRfcommChat server.",
                    NotifyType.ErrorMessage);
                RunButton.IsEnabled = true;
                return;
            }

            var serviceNameLength = attributeReader.ReadByte();

            // The Service Name attribute requires UTF-8 encoding.
            attributeReader.UnicodeEncoding = UnicodeEncoding.Utf8;
            ServiceName.Text = "Service Name: \"" + attributeReader.ReadString(serviceNameLength) + "\"";

            lock (this)
            {
                chatSocket = new StreamSocket();
            }
            try
            {
                await chatSocket.ConnectAsync(chatService.ConnectionHostName, chatService.ConnectionServiceName);

                chatWriter = new DataWriter(chatSocket.OutputStream);
                ChatBox.Visibility = Windows.UI.Xaml.Visibility.Visible;

                DataReader chatReader = new DataReader(chatSocket.InputStream);
                ReceiveStringLoop(chatReader);
            }
            catch (Exception ex)
            {
                switch ((uint)ex.HResult)
                {
                    case (0x80070490): // ERROR_ELEMENT_NOT_FOUND
                        rootPage.NotifyUser("Please verify that you are running the BluetoothRfcommChat server.", NotifyType.ErrorMessage);
                        RunButton.IsEnabled = true;
                        break;
                    default:
                        throw;
                }
            }
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
        
        /// <summary>
        /// Takes the contents of the MessageTextBox and writes it to the outgoing chatWriter
        /// </summary>
        private async void SendMessage()
        {
            try
            {
                if (MessageTextBox.Text.Length != 0)
                {
                    chatWriter.WriteUInt32((uint)MessageTextBox.Text.Length);
                    chatWriter.WriteString(MessageTextBox.Text);

                    ConversationList.Items.Add("Sent: " + MessageTextBox.Text);
                    MessageTextBox.Text = "";
                    await chatWriter.StoreAsync();
                    
                }
            }
            catch (Exception ex)
            {
                // TODO: Catch disconnect -  HResult = 0x80072745 - catch this (remote device disconnect) ex = {"An established connection was aborted by the software in your host machine. (Exception from HRESULT: 0x80072745)"}
                rootPage.NotifyUser("Error: " + ex.HResult.ToString() + " - " + ex.Message,
                    NotifyType.StatusMessage);
            }
        }        

        private async void ReceiveStringLoop(DataReader chatReader)
        {
            try
            {
                uint size = await chatReader.LoadAsync(sizeof(uint));
                if (size < sizeof(uint))
                {
                    Disconnect("Remote device terminated connection");
                    return;
                }

                uint stringLength = chatReader.ReadUInt32();
                uint actualStringLength = await chatReader.LoadAsync(stringLength);
                if (actualStringLength != stringLength)
                {
                    // The underlying socket was closed before we were able to read the whole data
                    return;
                }

                ConversationList.Items.Add("Received: " + chatReader.ReadString(stringLength));

                ReceiveStringLoop(chatReader);
            }
            catch (Exception ex)
            {
                lock (this)
                {
                    if (chatSocket == null)
                    {
                        // Do not print anything here -  the user closed the socket.
                        // HResult = 0x80072745 - catch this (remote device disconnect) ex = {"An established connection was aborted by the software in your host machine. (Exception from HRESULT: 0x80072745)"}
                    }
                    else
                    {
                        Disconnect("Read stream failed with error: " + ex.Message);
                    }
                }
            }
        }

        private void DisconnectButton_Click(object sender, RoutedEventArgs e)
        {
            Disconnect("Disconnected");
        }


        /// <summary>
        /// Cleans up the socket and DataWriter and reset the UI
        /// </summary>
        /// <param name="disconnectReason"></param>
        private void Disconnect(string disconnectReason)
        {
            if (chatWriter != null)
            {
                chatWriter.DetachStream();
                chatWriter = null;
            }


            if (chatService != null)
            {
                chatService.Dispose();
                chatService = null;
            }
            lock (this)
            {
                if (chatSocket != null)
                {
                    chatSocket.Dispose();
                    chatSocket = null;
                }
            }

            rootPage.NotifyUser(disconnectReason, NotifyType.StatusMessage);
            RunButton.IsEnabled = true;
            DeviceList.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            ChatBox.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            ConversationList.Items.Clear();
        }
    }
}
