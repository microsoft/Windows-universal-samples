// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "DeviceWatcherHelper.h"
#include "MainPage.xaml.h"

using namespace SDKTemplate;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::Devices::Enumeration;

void DeviceWatcherHelper::StartWatcher(DeviceWatcher^ deviceWatcherArg)
{
    deviceWatcher = deviceWatcherArg;

    // Connect events to update our collection as the watcher report results.
    deviceWatcher->Added += ref new TypedEventHandler<DeviceWatcher^, DeviceInformation^>(this, &DeviceWatcherHelper::Watcher_DeviceAdded);
    deviceWatcher->Updated += ref new TypedEventHandler<DeviceWatcher^, DeviceInformationUpdate^>(this, &DeviceWatcherHelper::Watcher_DeviceUpdated);
    deviceWatcher->Removed += ref new TypedEventHandler<DeviceWatcher^, DeviceInformationUpdate^>(this, &DeviceWatcherHelper::Watcher_DeviceRemoved);
    deviceWatcher->EnumerationCompleted += ref new TypedEventHandler<DeviceWatcher^, Object^>(this, &DeviceWatcherHelper::Watcher_EnumerationCompleted);
    deviceWatcher->Stopped += ref new TypedEventHandler<DeviceWatcher^, Object^>(this, &DeviceWatcherHelper::Watcher_Stopped);

    deviceWatcher->Start();
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
        deviceWatcher->Stop();
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

bool DeviceWatcherHelper::IsWatcherStarted(DeviceWatcher^ watcher)
{
    DeviceWatcherStatus status = watcher->Status;
    return (status == DeviceWatcherStatus::Started) ||
        (status == DeviceWatcherStatus::EnumerationCompleted);
}

bool DeviceWatcherHelper::IsWatcherRunning()
{
    if (deviceWatcher == nullptr)
    {
        return false;
    }

    DeviceWatcherStatus status = deviceWatcher->Status;
    return (status == DeviceWatcherStatus::Started) ||
        (status == DeviceWatcherStatus::EnumerationCompleted) ||
        (status == DeviceWatcherStatus::Stopping);
}

void DeviceWatcherHelper::Watcher_DeviceAdded(DeviceWatcher^ sender, DeviceInformation^ deviceInfo)
{
    // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
    dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
        [this, sender, deviceInfo]()
    {
        // Watcher may have stopped while we were waiting for our chance to run.
        if (IsWatcherStarted(sender))
        {
            resultCollection->Append(ref new DeviceInformationDisplay(deviceInfo));
            RaiseDeviceChanged(sender, deviceInfo->Id);
        }
    }));
}

void DeviceWatcherHelper::Watcher_DeviceUpdated(DeviceWatcher^ sender, DeviceInformationUpdate^ deviceInfoUpdate)
{
    // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
    dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
        [this, sender, deviceInfoUpdate]()
    {
        // Watcher may have stopped while we were waiting for our chance to run.
        if (IsWatcherStarted(sender))
        {
            // Find the corresponding updated DeviceInformation in the collection and pass the update object
            // to the Update method of the existing DeviceInformation. This automatically updates the object
            // for us.
            auto foundDeviceInfo = std::find_if(begin(resultCollection), end(resultCollection), [&](DeviceInformationDisplay^ di) { return (di->Id == deviceInfoUpdate->Id); });

            if (foundDeviceInfo != end(resultCollection))
            {
                (*foundDeviceInfo)->Update(deviceInfoUpdate);
                RaiseDeviceChanged(sender, deviceInfoUpdate->Id);
            }
        }
    }));
}

void DeviceWatcherHelper::Watcher_DeviceRemoved(DeviceWatcher^ sender, DeviceInformationUpdate^ deviceInfoUpdate)
{
    // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
    dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
        [this, sender, deviceInfoUpdate]()
    {
        // Watcher may have stopped while we were waiting for our chance to run.
        if (IsWatcherStarted(sender))
        {
            // Find the corresponding DeviceInformation in the collection and remove it
            auto foundDeviceInfo = std::find_if(begin(resultCollection), end(resultCollection), [&](DeviceInformationDisplay^ di) {return (di->Id == deviceInfoUpdate->Id); });

            uint32 index = 0;
            if (foundDeviceInfo != end(resultCollection) &&
                resultCollection->IndexOf(*foundDeviceInfo, &index))
            {
                resultCollection->RemoveAt(index);
            }

            RaiseDeviceChanged(sender, deviceInfoUpdate->Id);
        }
    }));
}

void DeviceWatcherHelper::Watcher_EnumerationCompleted(DeviceWatcher^ sender, Object^ e)
{
    dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
        [this, sender]()
    {
        RaiseDeviceChanged(sender, nullptr);
    }));
}

void DeviceWatcherHelper::Watcher_Stopped(DeviceWatcher^ sender, Object^ e)
{
    dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
        [this, sender]()
    {
        RaiseDeviceChanged(sender, nullptr);
    }));
}

void DeviceWatcherHelper::RaiseDeviceChanged(DeviceWatcher^ sender, String^ id)
{
    if (UpdateStatus)
    {
        // Display a default status message.
        String^ message = resultCollection->Size.ToString() + " devices found.";
        switch (sender->Status)
        {
        case DeviceWatcherStatus::Started:
            break;

        case DeviceWatcherStatus::EnumerationCompleted:
            message = message + " Enumeration completed. Watching for updates...";
            break;

        case DeviceWatcherStatus::Stopped:
            message = message + " Watcher stopped.";
            break;

        case DeviceWatcherStatus::Aborted:
            message = message + " Watcher aborted.";
            break;
        }

        MainPage::Current->NotifyUser(message, NotifyType::StatusMessage);
    }

    DeviceChanged(sender, id);
}

