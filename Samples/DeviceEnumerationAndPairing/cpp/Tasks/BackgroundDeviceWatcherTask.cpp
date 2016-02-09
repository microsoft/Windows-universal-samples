// Copyright (c) Microsoft Corporation. All rights reserved.

#include "pch.h"
#include "BackgroundDeviceWatcherTask.h"

using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::Devices::Enumeration;

void BackgroundDeviceWatcherTaskCpp::BackgroundDeviceWatcher::Run(IBackgroundTaskInstance^ taskInstance)
{
    String^ outputString;

    OutputDebugStringW(L"BackgroundDeviceWatcher::Run called\r\n");

    uint32 eventCount = 0;
    ApplicationDataContainer^ settings = ApplicationData::Current->LocalSettings;
    DeviceWatcherTriggerDetails^ triggerDetails = (DeviceWatcherTriggerDetails^)taskInstance->TriggerDetails;
    
    outputString = L"Trigger contains " + triggerDetails->DeviceWatcherEvents->Size + L" events\r\n";
    OutputDebugStringW(outputString->Data());

    std::for_each(begin(triggerDetails->DeviceWatcherEvents), end(triggerDetails->DeviceWatcherEvents), [&](DeviceWatcherEvent^ event)
    {
        switch (event->Kind)
        {
        case DeviceWatcherEventKind::Add:
            outputString = L"Add: " + event->DeviceInformation->Id + L"\r\n";
            OutputDebugStringW(outputString->Data());
            break;

        case DeviceWatcherEventKind::Update:
            outputString = L"Update: " + event->DeviceInformationUpdate->Id + L"\r\n";
            OutputDebugStringW(outputString->Data());
            break;

        case DeviceWatcherEventKind::Remove:
            outputString = L"Remove: " + event->DeviceInformationUpdate->Id + L"\r\n";
            OutputDebugStringW(outputString->Data());
            break;
        };
    });

    if (nullptr != settings->Values->Lookup("eventCount"))
    {
        eventCount = (uint32)settings->Values->Lookup("eventCount");
    }

    // Add the number of events for this trigger to the number of events received in the past
    eventCount += (uint32)triggerDetails->DeviceWatcherEvents->Size;

    outputString = eventCount + L" events process for the lifetime of the trigger\r\n";
    OutputDebugStringW(outputString->Data());

    settings->Values->Insert("eventCount", eventCount);
}
