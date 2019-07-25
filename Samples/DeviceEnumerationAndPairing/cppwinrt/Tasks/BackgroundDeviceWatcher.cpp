#include "pch.h"
#include "BackgroundDeviceWatcher.h"
#include "BackgroundDeviceWatcher.g.cpp"

using namespace winrt;
using namespace winrt::Windows::ApplicationModel::Background;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Storage;

namespace winrt::BackgroundDeviceWatcherTask::implementation
{
    void DebugWriteLine(const hstring& message)
    {
        OutputDebugStringW(message.c_str());
        OutputDebugStringW(L"\r\n");
    }

    void BackgroundDeviceWatcher::Run(IBackgroundTaskInstance const& taskInstance)
    {
        DebugWriteLine(L"BackgroundDeviceWatcher.Run called");

        ApplicationDataContainer settings = ApplicationData::Current().LocalSettings();
        DeviceWatcherTriggerDetails triggerDetails = taskInstance.TriggerDetails().as<DeviceWatcherTriggerDetails>();

        uint32_t newEventCount = triggerDetails.DeviceWatcherEvents().Size();
        DebugWriteLine(L"Trigger contains " + to_hstring(newEventCount) + L" events");

        for (DeviceWatcherEvent const& e : triggerDetails.DeviceWatcherEvents())
        {
            switch (e.Kind())
            {
            case DeviceWatcherEventKind::Add:
                DebugWriteLine(L"Add: " + e.DeviceInformation().Id());
                break;

            case DeviceWatcherEventKind::Update:
                DebugWriteLine(L"Update: " + e.DeviceInformationUpdate().Id());
                break;

            case DeviceWatcherEventKind::Remove:
                DebugWriteLine(L"Remove: " + e.DeviceInformationUpdate().Id());
                break;
            }
        }

        uint32_t eventCount = unbox_value_or<uint32_t>(settings.Values().TryLookup(L"eventCount"), 0);

        // Add the number of events for this trigger to the number of events received in the past
        eventCount += newEventCount;

        DebugWriteLine(to_hstring(eventCount) + L" events processed for lifetime of trigger");

        settings.Values().Insert(L"eventCount", box_value(eventCount));
    }
}
