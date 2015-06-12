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

#include <collection.h>
#include "WiFiDirectServicesWrappers.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    namespace WiFiDirectServices
    {
        ref class Scenario1;
        ref class Scenario2;
        ref class Scenario3;
        ref class Scenario4;
        ref class Scenario5;

        /// <summary>
        /// Common class to wrap all of the services objects and state behind the UI
        /// </summary>
        public ref class WiFiDirectServiceManager sealed
        {
        private:
            MainPage^ _rootPage;

            // UI Integration
            // Keep track of known UI frames and update them when we have any changes
            // NOTE: UI pages may not exist and the UI will need to update itself when it initializes
            Scenario1^ _scenario1 = nullptr;
            Scenario2^ _scenario2 = nullptr;
            Scenario3^ _scenario3 = nullptr;
            Scenario4^ _scenario4 = nullptr;
            Scenario5^ _scenario5 = nullptr;
            
            // Keep list of all advertised services currently active
            Windows::Foundation::Collections::IVector<AdvertisementWrapper^>^ _advertisers = ref new Platform::Collections::Vector<AdvertisementWrapper^>();

            // Keep track of all devices found during previous discovery
            Windows::Foundation::Collections::IVector<DiscoveredDeviceWrapper^>^ _discoveredDevices = ref new Platform::Collections::Vector<DiscoveredDeviceWrapper^>();

            // Keep track of all sessions that are connected to this device
            // NOTE: it may make sense to track sessions per-advertiser and a second list for the seeker, but this sample keeps a global list
            Windows::Foundation::Collections::IVector<SessionWrapper^>^ _connectedSessions = ref new Platform::Collections::Vector<SessionWrapper^>();

            // Keep track of the selected session for the UI
            // The UI will choose this session from the session list and use it for sending/receiving data
            int _selectedSession = -1;

            // Singleton
            static WiFiDirectServiceManager^ _instance;
            WiFiDirectServiceManager();

            std::mutex _managerMutex;

        internal:
            // Singleton access
            static property WiFiDirectServiceManager^ Instance
            {
                WiFiDirectServiceManager^ get();
            }

            void NotifyUser(Platform::String^ strMessage, NotifyType type);

            property MainPage^ RootPage
            {
                MainPage^ get();
            }

            property Scenario1^ CurrentScenario1
            {
                Scenario1^ get() { return _scenario1; }
                void set(Scenario1^ value) { _scenario1 = value; }
            }

            property Scenario2^ CurrentScenario2
            {
                Scenario2^ get() { return _scenario2; }
                void set(Scenario2^ value) { _scenario2 = value; }
            }

            property Scenario3^ CurrentScenario3
            {
                Scenario3^ get() { return _scenario3; }
                void set(Scenario3^ value) { _scenario3 = value; }
            }

            property Scenario4^ CurrentScenario4
            {
                Scenario4^ get() { return _scenario4; }
                void set(Scenario4^ value) { _scenario4 = value; }
            }

            property Scenario5^ CurrentScenario5
            {
                Scenario5^ get() { return _scenario5; }
                void set(Scenario5^ value) { _scenario5 = value; }
            }

            property Windows::Foundation::Collections::IVector<AdvertisementWrapper^>^ Advertisers
            {
                Windows::Foundation::Collections::IVector<AdvertisementWrapper^>^ get() { return _advertisers; }
            }

            property SessionWrapper^ CurrentSession
            {
                SessionWrapper^ get()
                {
                    if (_selectedSession >= 0 &&
                        static_cast<unsigned int>(_selectedSession) < _connectedSessions->Size)
                    {
                        return _connectedSessions->GetAt(_selectedSession);
                    }
                    return nullptr;
                }
            }

            property Windows::Foundation::Collections::IVector<DiscoveredDeviceWrapper^>^ DiscoveredDevices
            {
                Windows::Foundation::Collections::IVector<DiscoveredDeviceWrapper^>^ get() { return _discoveredDevices; }
            }

            property Windows::Foundation::Collections::IVector<SessionWrapper^>^ ConnectedSessionList
            {
                Windows::Foundation::Collections::IVector<SessionWrapper^>^ get() { return _connectedSessions; }
            }

            void StartAdvertisement(
                Platform::String^ serviceName,
                bool autoAccept,
                bool preferGO,
                Platform::String^ pin,
                Windows::Foundation::Collections::IVector<Windows::Devices::WiFiDirect::Services::WiFiDirectServiceConfigurationMethod>^ configMethods,
                Windows::Devices::WiFiDirect::Services::WiFiDirectServiceStatus status,
                unsigned int customStatus,
                Platform::String^ serviceInfo,
                Platform::String^ deferredServiceInfo,
                Windows::Foundation::Collections::IVector<Platform::String^>^ prefixList
                );

            void UnpublishService(unsigned int index);

            void DiscoverServicesAsync(
                Platform::String^ serviceName,
                Platform::String^ requestedServiceInfo = ""
                );

            void RemoveAdvertiser(AdvertisementWrapper^ advertiser);

            void AddSessionRequest(
                Windows::Devices::WiFiDirect::Services::WiFiDirectServiceSessionRequest^ request,
                AdvertisementWrapper^ advertiser
                );

            void RemoveSessionRequest(
                Windows::Devices::WiFiDirect::Services::WiFiDirectServiceSessionRequest^ request,
                AdvertisementWrapper^ advertiser
                );

            void AdvertiserStatusChanged(AdvertisementWrapper^ advertiser);

            void AddSession(SessionWrapper^ session);

            void RemoveSession(unsigned int index);

            void RemoveSession(SessionWrapper^ session);

            void CloseSession(unsigned int index);

            void SelectSession(unsigned int index);

            void AddSocket(SocketWrapper^ socket);

        private:
            void AddAdvertiser(AdvertisementWrapper^ advertiser);

            void RemoveAdvertiser(unsigned int index);
        };
    }
}