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

#pragma once

#include <condition_variable>

namespace SDKTemplate
{
    namespace WiFiDirectServices
    {
        ref class WiFiDirectServiceManager;

        /// <summary>
        /// Wraps WiFiDirectServiceAdvertiser and handles callbacks and related state
        /// </summary>
        [Windows::UI::Xaml::Data::Bindable]
        public ref class AdvertisementWrapper sealed
        {
        private:
            Windows::Devices::WiFiDirect::Services::WiFiDirectServiceAdvertiser^ _advertiser = nullptr;
            // Used to update main state
            WiFiDirectServiceManager^ _manager = nullptr;

            std::mutex _advertiserMutex;

            // PIN to use for display config method (if selected)
            Platform::String^ _pin = "";

            // Keep track of incoming service requests that need to be accepted or rejected
            Windows::Foundation::Collections::IVector<Windows::Devices::WiFiDirect::Services::WiFiDirectServiceSessionRequest^>^ _requestList
                = ref new Platform::Collections::Vector<Windows::Devices::WiFiDirect::Services::WiFiDirectServiceSessionRequest^>();

            // Keep internal identifier
            int _uniqueInternalId;
            static int _nextId;

            // Keep tokens to cleanup event handlers
            Windows::Foundation::EventRegistrationToken _advertisementStatusChangedToken;
            Windows::Foundation::EventRegistrationToken _autoAcceptSessionConnectedToken;
            Windows::Foundation::EventRegistrationToken _sessionRequestedToken;

        public:
            AdvertisementWrapper(
                Windows::Devices::WiFiDirect::Services::WiFiDirectServiceAdvertiser^ advertiser,
                WiFiDirectServiceManager^ manager,
                Platform::String^ pin
                );
            virtual ~AdvertisementWrapper();

            property Windows::Devices::WiFiDirect::Services::WiFiDirectServiceAdvertiser^ Advertiser
            {
                Windows::Devices::WiFiDirect::Services::WiFiDirectServiceAdvertiser^ get() { return _advertiser; }
            }

            property int InternalId
            {
                int get() { return _uniqueInternalId; }
            }

            property Platform::String^ Pin
            {
                Platform::String^ get() { return _pin; }
            }

            property Platform::String^ Name
            {
                Platform::String^ get() { return _advertiser->ServiceName; }
            }

            property Platform::String^ Status
            {
                Platform::String^ get() { return _advertiser->AdvertisementStatus.ToString(); }
            }

            property Windows::Foundation::Collections::IVector<Windows::Devices::WiFiDirect::Services::WiFiDirectServiceSessionRequest^>^ SessionRequestList
            {
                Windows::Foundation::Collections::IVector<Windows::Devices::WiFiDirect::Services::WiFiDirectServiceSessionRequest^>^ get()
                {
                    return _requestList;
                }
            }

        internal:
            void AcceptSessionRequest(Platform::String^ id, Platform::String^ pin = "");

            void DeclineSessionRequest(Platform::String^ id);

        private:
            // NOTE: this is mutually exclusive with OnAutoAcceptSessionConnected
            void OnSessionRequested(
                Windows::Devices::WiFiDirect::Services::WiFiDirectServiceAdvertiser^ sender,
                Windows::Devices::WiFiDirect::Services::WiFiDirectServiceSessionRequestedEventArgs^ args
                );

            // NOTE: this is mutually exclusive with OnSessionRequested
            void OnAutoAcceptSessionConnected(
                Windows::Devices::WiFiDirect::Services::WiFiDirectServiceAdvertiser^ sender,
                Windows::Devices::WiFiDirect::Services::WiFiDirectServiceAutoAcceptSessionConnectedEventArgs^ args
                );

            void OnAdvertisementStatusChanged(
                Windows::Devices::WiFiDirect::Services::WiFiDirectServiceAdvertiser^ sender,
                Platform::Object^ args
                );
        };

        [Windows::UI::Xaml::Data::Bindable]
        public ref class DiscoveredDeviceWrapper sealed
        {
        private:
            Windows::Devices::Enumeration::DeviceInformation^ _deviceInfo;
            // Used to update main state
            WiFiDirectServiceManager^ _manager = nullptr;

            // When we start opening/connecting to a session we store this (only used for a single connect)
            Windows::Devices::WiFiDirect::Services::WiFiDirectService^ _service = nullptr;

            // Keep tokens to cleanup event handlers
            Windows::Foundation::EventRegistrationToken _sessionDeferredToken;
        public:
            DiscoveredDeviceWrapper(
                Windows::Devices::Enumeration::DeviceInformation^ deviceInfo,
                WiFiDirectServiceManager^ manager
                );
            virtual ~DiscoveredDeviceWrapper();

            // Expose properties from the discovered device
            property Platform::String^ ServiceName;
            property Platform::String^ ServiceAddress;
            property unsigned int AdvertisementId;
            property unsigned int ConfigMethods;
            property Platform::String^ ServiceInfo;

        internal:
            /// <summary>
            /// Calls WiFiDirectService.FromId to open the seeker session
            /// Caller can then use this.Service to modify settings before connecting
            /// </summary>
            Concurrency::task<void> OpenSessionAsync();

            /// <summary>
            /// Call this to set options before doing the actual connect
            /// </summary>
            /// <param name="preferGO">If true, this peer will prefer to be the group owner (GO) of the connection</param>
            /// <param name="sessionInfo">Optional 144 byte buffer to send with the request to the advertising peer</param>
            Concurrency::task<void> SetServiceOptionsAsync(bool preferGO, Platform::String^ sessionInfo);

            /// <summary>
            /// Calls service.GetProvisioningInfoAsync to begin provision discovery and
            /// determine if a PIN will be needed to connect
            /// </summary>
            /// <param name="configMethod">Selected Config Method to use, based on supported config methods of discovered device</param>
            Concurrency::task<Windows::Devices::WiFiDirect::Services::WiFiDirectServiceProvisioningInfo^> GetProvisioningInfoAsync(
                Windows::Devices::WiFiDirect::Services::WiFiDirectServiceConfigurationMethod configMethod
                );

            /// <summary>
            /// Calls service.ConnectAsync() to connect the session
            /// If the session is not already opened, it calls OpenSession
            /// </summary>
            /// <param name="pin">Optional PIN if needed</param>
            Concurrency::task<void> ConnectAsync(Platform::String^ pin = "");

        private:
            void OnSessionDeferred(
                Windows::Devices::WiFiDirect::Services::WiFiDirectService^ sender,
                Windows::Devices::WiFiDirect::Services::WiFiDirectServiceSessionDeferredEventArgs^ args
                );

            void ParseProperties();
        };

        ref class SessionWrapper;

        public ref class StreamSocketListenerWrapper sealed
        {
        private:
            Windows::Networking::Sockets::StreamSocketListener^ _streamSocketListener;
            // Used to update main state
            WiFiDirectServiceManager^ _manager = nullptr;

            SessionWrapper^ _session = nullptr;

            // Keep tokens to cleanup event handlers
            Windows::Foundation::EventRegistrationToken _connectionReceivedToken;

        public:
            StreamSocketListenerWrapper(
                WiFiDirectServiceManager^ manager,
                Windows::Networking::Sockets::StreamSocketListener^ streamSocketListener,
                SessionWrapper^ session
                );
            virtual ~StreamSocketListenerWrapper();

        private:
            void OnConnectionReceived(
                Windows::Networking::Sockets::StreamSocketListener^ sender,
                Windows::Networking::Sockets::StreamSocketListenerConnectionReceivedEventArgs^ args
                );
        };

        // Generic wrapper for TCP (Stream) or UDP (Datagram) socket
        // For this sample, this just allows messages to be sent between connected peers,
        // real application would handle higher level logic over the connected socket(s)
        [Windows::UI::Xaml::Data::Bindable]
        public ref class SocketWrapper sealed
        {
        private:
            Windows::Networking::Sockets::StreamSocket^ _streamSocket = nullptr;
            Windows::Networking::Sockets::DatagramSocket^ _datagramSocket = nullptr;
            // Used to update main state
            WiFiDirectServiceManager^ _manager = nullptr;

            Windows::Storage::Streams::DataReader^ _reader = nullptr;
            Windows::Storage::Streams::DataWriter^ _writer = nullptr;

            // Keep tokens to cleanup event handlers
            Windows::Foundation::EventRegistrationToken _datagramMessageReceivedToken;

        public:
            SocketWrapper(
                WiFiDirectServiceManager^ manager,
                Windows::Networking::Sockets::StreamSocket^ streamSocket,
                Windows::Networking::Sockets::DatagramSocket^ datagramSocket,
                bool canSend
                );
            virtual ~SocketWrapper();

            property Platform::String^ Protocol
            {
                Platform::String^ get();
            }
            property Platform::String^ Port
            {
                Platform::String^ get();
            }

        internal:
            /// <summary>
            /// Send a string over the socket (TCP or UDP)
            /// Will send as a uint32 size followed by the message
            /// </summary>
            /// <param name="message">Message to send</param>
            Concurrency::task<void> SendMessageAsync(Platform::String^ message);

            void StartReceiveLoop();

            /// <summary>
            /// Read strings sent over the TCP socket
            /// Reads the uint32 size then the actual string
            /// </summary>
            Concurrency::task<void> ReceiveMessageAsync();

        private:

            void OnUDPMessageReceived(
                Windows::Networking::Sockets::DatagramSocket^ sender,
                Windows::Networking::Sockets::DatagramSocketMessageReceivedEventArgs^ args
                );
        };

        [Windows::UI::Xaml::Data::Bindable]
        public ref class SessionWrapper sealed
        {
        private:
            Windows::Devices::WiFiDirect::Services::WiFiDirectServiceSession^ _session;
            // Used to update main state
            WiFiDirectServiceManager^ _manager = nullptr;

            // Stream Socket Listeners are created when locally opening TCP ports
            Windows::Foundation::Collections::IVector<StreamSocketListenerWrapper^>^ _streamSocketListeners
                = ref new Platform::Collections::Vector<StreamSocketListenerWrapper^>();

            // Keep a list of connected sockets
            Windows::Foundation::Collections::IVector<SocketWrapper^>^ _socketList
                = ref new Platform::Collections::Vector<SocketWrapper^>();

            // Keep tokens to cleanup event handlers
            Windows::Foundation::EventRegistrationToken _remotePortAddedToken;
            Windows::Foundation::EventRegistrationToken _sessionStatusChangedToken;

            // Used to wait for the session to close before cleaning up the wrapper
            Concurrency::event _sessionClosedEvent;

            // Protect changes to the _session member
            std::mutex _sessionMutex;

        public:
            SessionWrapper(
                Windows::Devices::WiFiDirect::Services::WiFiDirectServiceSession^ session,
                WiFiDirectServiceManager^ manager);
            virtual ~SessionWrapper();

            property Windows::Devices::WiFiDirect::Services::WiFiDirectServiceSession^ Session
            {
                Windows::Devices::WiFiDirect::Services::WiFiDirectServiceSession^ get()
                {
                    std::lock_guard<std::mutex> lock(_sessionMutex);
                    return _session;
                }
            }

            property unsigned int AdvertisementId
            {
                unsigned int get() { return _session->AdvertisementId; }
            }
            property unsigned int SessionId
            {
                unsigned int get() { return _session->SessionId; }
            }

            property Platform::String^ ServiceAddress
            {
                Platform::String^ get() { return _session->ServiceAddress; }
            }

            property Platform::String^ SessionAddress
            {
                Platform::String^ get() { return _session->SessionAddress; }
            }

            property Windows::Foundation::Collections::IVector<SocketWrapper^>^ SocketList
            {
                Windows::Foundation::Collections::IVector<SocketWrapper^>^ get() { return _socketList; }
            }

        internal:
            void Close();

            Concurrency::task<void> AddStreamSocketListenerAsync(uint16 port);

            Concurrency::task<void> AddDatagramSocketAsync(uint16 port);

            void AddStreamSocketInternal(Windows::Networking::Sockets::StreamSocket^ socket);

        private:
            // This will fire when the connected peer attempts to open a port for this connection
            // The peer should start a stream socket listener (for TCP)
            void OnRemotePortAdded(
                Windows::Devices::WiFiDirect::Services::WiFiDirectServiceSession^ sender,
                Windows::Devices::WiFiDirect::Services::WiFiDirectServiceRemotePortAddedEventArgs^ args
                );

            void OnSessionStatusChanged(
                Windows::Devices::WiFiDirect::Services::WiFiDirectServiceSession^ sender,
                Platform::Object^ args
                );
        };

    }
}