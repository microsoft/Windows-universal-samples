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
using Windows.Networking.Sockets;
using Windows.Storage.Streams;
using Windows.Devices.Enumeration;
using System.ComponentModel;
using Windows.Devices.WiFiDirect;

namespace SDKTemplate
{
    public static class Globals
    {
        public static readonly byte[] CustomOui = { 0xAA, 0xBB, 0xCC };
        public static readonly byte CustomOuiType = 0xDD;
        public static readonly byte[] WfaOui = { 0x50, 0x6F, 0x9A };
        public static readonly byte[] MsftOui = { 0x00, 0x50, 0xF2 };
        public static readonly string strServerPort = "50001";
    }

    public class SocketReaderWriter : IDisposable
    {
        DataReader _dataReader;
        DataWriter _dataWriter;
        StreamSocket _streamSocket;
        private MainPage _rootPage;
        string _currentMessage;

        public SocketReaderWriter(StreamSocket socket, MainPage mainPage)
        {
            _dataReader = new DataReader(socket.InputStream);
            _dataReader.UnicodeEncoding = UnicodeEncoding.Utf8;
            _dataReader.ByteOrder = ByteOrder.LittleEndian;

            _dataWriter = new DataWriter(socket.OutputStream);
            _dataWriter.UnicodeEncoding = UnicodeEncoding.Utf8;
            _dataWriter.ByteOrder = ByteOrder.LittleEndian;

            _streamSocket = socket;
            _rootPage = mainPage;
            _currentMessage = null;
        }

        public void Dispose()
        {
            _dataReader.Dispose();
            _dataWriter.Dispose();
            _streamSocket.Dispose();
        }

        public async void WriteMessage(string message)
        {
            try
            {
                _dataWriter.WriteUInt32(_dataWriter.MeasureString(message));
                _dataWriter.WriteString(message);
                await _dataWriter.StoreAsync();
                _rootPage.NotifyUserFromBackground("Sent message: " + message, NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                _rootPage.NotifyUserFromBackground("WriteMessage threw exception: " + ex.Message, NotifyType.StatusMessage);
            }
        }

        public async void ReadMessage()
        {
            try
            {
                UInt32 bytesRead = await _dataReader.LoadAsync(sizeof(UInt32));
                if (bytesRead > 0)
                {
                    // Determine how long the string is.
                    UInt32 messageLength = _dataReader.ReadUInt32();
                    bytesRead = await _dataReader.LoadAsync(messageLength);
                    if (bytesRead > 0)
                    {
                        // Decode the string.
                        _currentMessage = _dataReader.ReadString(messageLength);
                        _rootPage.NotifyUserFromBackground("Got message: " + _currentMessage, NotifyType.StatusMessage);

                        ReadMessage();
                    }
                }
            }
            catch (Exception)
            {
                _rootPage.NotifyUserFromBackground("Socket was closed!", NotifyType.StatusMessage);
            }
        }

        public string GetCurrentMessage()
        {
            return _currentMessage;
        }
    }

    public class DiscoveredDevice : INotifyPropertyChanged
    {
        private DeviceInformation deviceInfo;

        public DiscoveredDevice(DeviceInformation deviceInfoIn)
        {
            deviceInfo = deviceInfoIn;
        }

        public DeviceInformation DeviceInfo
        {
            get
            {
                return deviceInfo;
            }
        }

        public override string ToString()
        {
            return deviceInfo.Name;
        }

        public event PropertyChangedEventHandler PropertyChanged;
    }

    public class ConnectedDevice : INotifyPropertyChanged
    {
        private SocketReaderWriter socketRW;
        private WiFiDirectDevice wfdDevice;
        private string displayName = "";

        public ConnectedDevice(string displayName, WiFiDirectDevice wfdDevice, SocketReaderWriter socketRW)
        {
            this.socketRW = socketRW;
            this.wfdDevice = wfdDevice;
            this.displayName = displayName;
        }

        private ConnectedDevice() { }

    public SocketReaderWriter SocketRW
        {
            get
            {
                return socketRW;
            }

            set
            {
                socketRW = value;
            }
        }

        public WiFiDirectDevice WfdDevice
        {
            get
            {
                return wfdDevice;
            }

            set
            {
                wfdDevice = value;
            }
        }

        public override string ToString()
        {
            return displayName;
        }

        public string DisplayName
        {
            get
            {
                return displayName;
            }

            set
            {
                displayName = value;
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
    }
}
