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

namespace winrt::SDKTemplate
{
    struct DeviceWatcherHelper : implements<DeviceWatcherHelper, Windows::Foundation::IInspectable>
    {
        DeviceWatcherHelper(
            Windows::Foundation::Collections::IObservableVector<SDKTemplate::DeviceInformationDisplay> const& resultCollection,
            Windows::UI::Core::CoreDispatcher const& dispatcher) :
            resultCollection(resultCollection), dispatcher(dispatcher) { }

        event_token DeviceChanged(Windows::Foundation::TypedEventHandler<Windows::Devices::Enumeration::DeviceWatcher, hstring> const& handler)
        {
            return deviceChanged.add(handler);
        }

        void DeviceChanged(event_token const& token)
        {
            deviceChanged.remove(token);
        };

        void UpdateStatus(bool update)
        {
            updateStatus = update;
        }

        void StartWatcher(Windows::Devices::Enumeration::DeviceWatcher const& deviceWatcher);
        void StopWatcher();
        void Reset();
        bool IsWatcherRunning();

    private:
        Windows::Devices::Enumeration::DeviceWatcher deviceWatcher{ nullptr };
        Windows::Foundation::Collections::IObservableVector<SDKTemplate::DeviceInformationDisplay> resultCollection{ nullptr };
        Windows::UI::Core::CoreDispatcher dispatcher{ nullptr };
        bool updateStatus = true;
        event<Windows::Foundation::TypedEventHandler<Windows::Devices::Enumeration::DeviceWatcher, hstring>> deviceChanged;

        static bool IsWatcherStarted(Windows::Devices::Enumeration::DeviceWatcher const& deviceWatcher);
        fire_and_forget Watcher_DeviceAdded(Windows::Devices::Enumeration::DeviceWatcher sender, Windows::Devices::Enumeration::DeviceInformation deviceInfo);
        fire_and_forget Watcher_DeviceUpdated(Windows::Devices::Enumeration::DeviceWatcher sender, Windows::Devices::Enumeration::DeviceInformationUpdate deviceInfoUpdate);
        fire_and_forget Watcher_DeviceRemoved(Windows::Devices::Enumeration::DeviceWatcher sender, Windows::Devices::Enumeration::DeviceInformationUpdate deviceInfoUpdate);
        fire_and_forget Watcher_EnumerationCompleted(Windows::Devices::Enumeration::DeviceWatcher sender, IInspectable const&);
        fire_and_forget Watcher_Stopped(Windows::Devices::Enumeration::DeviceWatcher sender, IInspectable const&);
        void RaiseDeviceChanged(Windows::Devices::Enumeration::DeviceWatcher const& sender, hstring const& id);
    };
}