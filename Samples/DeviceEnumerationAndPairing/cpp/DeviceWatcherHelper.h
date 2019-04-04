// Copyright (c) Microsoft. All rights reserved.

#pragma once
#include "DisplayHelpers.h"

namespace SDKTemplate
{
    /// <summary>
    /// Updates an IObservableVector based on events from a DeviceWatcher.
    /// </summary>
    /// <remarks>
    /// Encapsulates the work necessary to register for watcher events,
    /// start and stop the watcher, handle race conditions, and break cycles.
    /// </remarks>
    public ref class DeviceWatcherHelper sealed
    {
    public:
        DeviceWatcherHelper(
            Windows::Foundation::Collections::IObservableVector<DeviceInformationDisplay^>^ resultCollection,
            Windows::UI::Core::CoreDispatcher^ dispatcher) :
            resultCollection(resultCollection),
            dispatcher(dispatcher)
        {
            UpdateStatus = true;
        }

        event Windows::Foundation::TypedEventHandler<Windows::Devices::Enumeration::DeviceWatcher^, Platform::String^>^ DeviceChanged;
        property bool UpdateStatus;

        void StartWatcher(Windows::Devices::Enumeration::DeviceWatcher^ deviceWatcher);
        void StopWatcher();
        void Reset();
        bool IsWatcherRunning();

    private:
        Windows::Devices::Enumeration::DeviceWatcher^ deviceWatcher;
        Windows::Foundation::Collections::IObservableVector<DeviceInformationDisplay^>^ resultCollection;
        Windows::UI::Core::CoreDispatcher^ dispatcher;

        static bool IsWatcherStarted(Windows::Devices::Enumeration::DeviceWatcher^ deviceWatcher);
        void Watcher_DeviceAdded(Windows::Devices::Enumeration::DeviceWatcher^ deviceWatcher, Windows::Devices::Enumeration::DeviceInformation^ deviceInfo);
        void Watcher_DeviceUpdated(Windows::Devices::Enumeration::DeviceWatcher^ deviceWatcher, Windows::Devices::Enumeration::DeviceInformationUpdate^ deviceInfoUpdate);
        void Watcher_DeviceRemoved(Windows::Devices::Enumeration::DeviceWatcher^ deviceWatcher, Windows::Devices::Enumeration::DeviceInformationUpdate^ deviceInfoUpdate);
        void Watcher_EnumerationCompleted(Windows::Devices::Enumeration::DeviceWatcher^ deviceWatcher, Platform::Object^ e);
        void Watcher_Stopped(Windows::Devices::Enumeration::DeviceWatcher^ deviceWatcher, Platform::Object^ e);
        void RaiseDeviceChanged(Windows::Devices::Enumeration::DeviceWatcher^ deviceWatcher, Platform::String^ id);
    };
}