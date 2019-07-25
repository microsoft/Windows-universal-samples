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

#include "pch.h"
#include "MainPage.h"
#include "SampleConfiguration.h"
#include "DeviceHelpers.h"
#include "DeviceWatcherHelper.h"

using namespace winrt;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Core;

namespace winrt::SDKTemplate
{
    void DeviceWatcherHelper::StartWatcher(DeviceWatcher const& deviceWatcherArg)
    {
        deviceWatcher = deviceWatcherArg;

        // Connect events to update our collection as the watcher report results.
        deviceWatcher.Added({ get_weak(), &DeviceWatcherHelper::DeviceWatcherHelper::Watcher_DeviceAdded });
        deviceWatcher.Updated({ get_weak(), &DeviceWatcherHelper::Watcher_DeviceUpdated });
        deviceWatcher.Removed({ get_weak(), &DeviceWatcherHelper::Watcher_DeviceRemoved });
        deviceWatcher.EnumerationCompleted({ get_weak(), &DeviceWatcherHelper::Watcher_EnumerationCompleted });
        deviceWatcher.Stopped({ get_weak(), &DeviceWatcherHelper::Watcher_Stopped });

        deviceWatcher.Start();
    }

    void DeviceWatcherHelper::StopWatcher()
    {
        // Since the device watcher runs in the background, it is possible that
        // a notification is "in flight" at the time we stop the watcher.
        // In other words, it is possible for the watcher to become stopped while a
        // handler is running, or for a handler to run after the watcher has stopped.

        if (IsWatcherStarted(deviceWatcher))
        {
            // We do not null out the deviceWatcher yet because we want to receive
            // the Stopped event.
            deviceWatcher.Stop();
        }
    }

    void DeviceWatcherHelper::Reset()
    {
        if (deviceWatcher != nullptr)
        {
            StopWatcher();
            deviceWatcher = nullptr;
        }
    }

    bool DeviceWatcherHelper::IsWatcherStarted(DeviceWatcher const& watcher)
    {
        DeviceWatcherStatus status = watcher.Status();
        return (status == DeviceWatcherStatus::Started) ||
            (status == DeviceWatcherStatus::EnumerationCompleted);
    }

    bool DeviceWatcherHelper::IsWatcherRunning()
    {
        if (deviceWatcher == nullptr)
        {
            return false;
        }

        DeviceWatcherStatus status = deviceWatcher.Status();
        return (status == DeviceWatcherStatus::Started) ||
            (status == DeviceWatcherStatus::EnumerationCompleted) ||
            (status == DeviceWatcherStatus::Stopping);
    }

    fire_and_forget DeviceWatcherHelper::Watcher_DeviceAdded(DeviceWatcher sender, DeviceInformation deviceInfo)
    {
        auto lifetime = get_strong();

        // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
        co_await resume_foreground(dispatcher);

        // Watcher may have stopped while we were waiting for our chance to run.
        if (IsWatcherStarted(sender))
        {
            resultCollection.Append(make<implementation::DeviceInformationDisplay>(deviceInfo));
            RaiseDeviceChanged(sender, deviceInfo.Id());
        }
    }

    fire_and_forget DeviceWatcherHelper::Watcher_DeviceUpdated(DeviceWatcher sender, DeviceInformationUpdate deviceInfoUpdate)
    {
        auto lifetime = get_strong();

        // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
        co_await resume_foreground(dispatcher);

        // Watcher may have stopped while we were waiting for our chance to run.
        if (IsWatcherStarted(sender))
        {
            // Find the corresponding updated DeviceInformation in the collection and pass the update object
            // to the Update method of the existing DeviceInformation. This automatically updates the object
            // for us.
            auto foundDeviceInfo = std::find_if(begin(resultCollection), end(resultCollection),
                [&](SDKTemplate::DeviceInformationDisplay const& di)
                {
                    return di.Id() == deviceInfoUpdate.Id();
                });

            if (foundDeviceInfo != end(resultCollection))
            {
                FromInspectable<implementation::DeviceInformationDisplay>(*foundDeviceInfo)->Update(deviceInfoUpdate);
                RaiseDeviceChanged(sender, deviceInfoUpdate.Id());
            }
        }
    }

    fire_and_forget DeviceWatcherHelper::Watcher_DeviceRemoved(DeviceWatcher sender, DeviceInformationUpdate deviceInfoUpdate)
    {
        auto lifetime = get_strong();

        // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
        co_await resume_foreground(dispatcher);

        // Watcher may have stopped while we were waiting for our chance to run.
        if (IsWatcherStarted(sender))
        {
            // Find the corresponding DeviceInformation in the collection and remove it
            auto foundDeviceInfo = std::find_if(begin(resultCollection), end(resultCollection),
                [&](SDKTemplate::DeviceInformationDisplay const& di)
                {
                    return di.Id() == deviceInfoUpdate.Id();
                });

            uint32_t index = 0;
            if (foundDeviceInfo != end(resultCollection) &&
                resultCollection.IndexOf(*foundDeviceInfo, index))
            {
                resultCollection.RemoveAt(index);
            }

            RaiseDeviceChanged(sender, deviceInfoUpdate.Id());
        }
    }
    fire_and_forget DeviceWatcherHelper::Watcher_EnumerationCompleted(DeviceWatcher sender, IInspectable const&)
    {
        auto lifetime = get_strong();

        co_await resume_foreground(dispatcher);
        RaiseDeviceChanged(sender, {});
    }

    fire_and_forget DeviceWatcherHelper::Watcher_Stopped(DeviceWatcher sender, IInspectable const&)
    {
        auto lifetime = get_strong();

        co_await resume_foreground(dispatcher);
        RaiseDeviceChanged(sender, {});
    }

    void DeviceWatcherHelper::RaiseDeviceChanged(DeviceWatcher const& sender, hstring const& id)
    {
        if (updateStatus)
        {
            // Display a default status message.
            hstring message = to_hstring(resultCollection.Size()) + L" devices found.";
            switch (sender.Status())
            {
            case DeviceWatcherStatus::Started:
                break;

            case DeviceWatcherStatus::EnumerationCompleted:
                message = message + L" Enumeration completed. Watching for updates...";
                break;

            case DeviceWatcherStatus::Stopped:
                message = message + L" Watcher stopped.";
                break;

            case DeviceWatcherStatus::Aborted:
                message = message + L" Watcher aborted.";
                break;
            }

            SDKTemplate::implementation::MainPage::Current().NotifyUser(message, NotifyType::StatusMessage);
        }

        deviceChanged(sender, id);
    }
}