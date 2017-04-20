#include "pch.h"
#include "DeviceList.h"
#include "Fx2Driver.h"
#include "MainPage.xaml.h"
#include "App.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::Custom;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Documents;

DeviceList^ DeviceList::_Current = nullptr;

DeviceList^ DeviceList::Current::get()
{
    if (DeviceList::_Current == nullptr) 
    {
        DeviceList::_Current = ref new DeviceList();
    }
    return DeviceList::_Current;
}

DeviceList::DeviceList() : m_WatcherStarted(false), m_WatcherSuspended(false)
{
    m_Fx2Watcher = nullptr;
    m_List = ref new Vector<DeviceListEntry^>();
    InitDeviceWatcher();

    // Register for app suspend/resume handlers
    App::Current->Suspending += ref new SuspendingEventHandler(this, &DeviceList::SuspendDeviceWatcher);
    App::Current->Resuming += ref new EventHandler<Object^>(this, &DeviceList::ResumeDeviceWatcher);
}

void DeviceList::InitDeviceWatcher()
{
    // Define the selector to enumerate all of the fx2 device interface class instances.
    // Use the DeviceInterfaceGuid provided by the driver (Fx2Driver, in this case).
    auto selector = CustomDevice::GetDeviceSelector(Fx2Driver::DeviceInterfaceGuid);

    // Set of properties to retrieve
    auto properties = ref new Vector<String^>({ "System.Devices.DeviceInstanceId" });

    // Create a device watcher to look for instances of the fx2 device interface.
    m_Fx2Watcher = DeviceInformation::CreateWatcher(selector, properties);

    m_Fx2Watcher->Added += ref new TypedEventHandler<DeviceWatcher^, DeviceInformation^>(this, &DeviceList::OnFx2Added);
    m_Fx2Watcher->Removed += ref new TypedEventHandler<DeviceWatcher^, DeviceInformationUpdate^>(this, &DeviceList::OnFx2Removed);
    m_Fx2Watcher->EnumerationCompleted += ref new TypedEventHandler<DeviceWatcher^, Object^>(this, &DeviceList::OnFx2EnumerationComplete);
}

void DeviceList::StartFx2Watcher() 
{
    MainPage::Current->NotifyUser("starting device watcher", NotifyType::StatusMessage);

    std::for_each(
        begin(m_List),
        end(m_List),
        [](DeviceListEntry^ Entry) {
        Entry->Matched = false;
    });

    WatcherStarted = true;
    m_Fx2Watcher->Start();
}

void DeviceList::StopFx2Watcher() 
{
    MainPage::Current->NotifyUser("stopping fx2 watcher", NotifyType::StatusMessage);
    m_Fx2Watcher->Stop();
    WatcherStarted = false;
}

void DeviceList::CreateBooleanTable(
    InlineCollection^ Table,
    const Platform::Array<bool>^ NewValues,
    const Platform::Array<bool>^ OldValues,
    String^ /* IndexTitle */,
    String^ /* ValueTitle */,
    String^ TrueValue,
    String^ FalseValue)
{
    Table->Clear();

    for (int i = 0; i < (int)NewValues->Length; i += 1) 
    {
        auto line = ref new Span();
        auto block = ref new Run();
        block->Text = (i + 1).ToString();
        line->Inlines->Append(block);

        block = ref new Run();
        block->Text = "    ";
        line->Inlines->Append(block);

        block = ref new Run();
        block->Text = NewValues[i] ? TrueValue : FalseValue;

        if ((OldValues != nullptr) && (OldValues[i] != NewValues[i])) 
        {
            auto bold = ref new Bold();
            bold->Inlines->Append(block);
            line->Inlines->Append(bold);
        }
        else 
        {
            line->Inlines->Append(block);
        }

        line->Inlines->Append(ref new LineBreak());

        Table->Append(line);
    }
}

DeviceListEntry^ DeviceList::FindDevice(String^ Id)
{
    auto i = std::find_if(
        begin(m_List),
        end(m_List),
        [Id](DeviceListEntry^ e) {return e->Id == Id; });

    if (i == end(m_List)) 
    {
        return nullptr;
    }
    else 
    {
        return *i;
    }
}

void DeviceList::OnFx2Added(DeviceWatcher ^ /* Sender */, DeviceInformation^ DevInterface)
{
    MainPage::Current->Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler(
            [this, DevInterface]()->void 
    {
        MainPage::Current->NotifyUser("OnFx2Added: " + DevInterface->Id, NotifyType::StatusMessage);

        // search the device list for a device with a matching interface ID
        auto match = FindDevice(DevInterface->Id);

        // If we found a match then mark it as verified and return
        if (match != nullptr) 
        {
            match->Matched = true;
            return;
        }

        // Create a new elemetn for this device interface, and queue up the query of its
        // device information
        match = ref new DeviceListEntry(DevInterface);

        // Add the new element to the end of the list of devices
        m_List->Append(match);
    }));
}

void DeviceList::OnFx2Removed(DeviceWatcher ^ /* Sender */, DeviceInformationUpdate^ DevInterface)
{
    auto deviceId = DevInterface->Id;

    MainPage::Current->Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler(
            [this, deviceId]() 
    {
        MainPage::Current->NotifyUser("OnFx2Removed: " + deviceId, NotifyType::StatusMessage);

        // Search the list of devices for one with a matching ID.  Move the matched 
        // item to the end of the list.
        auto i = std::remove_if(
            begin(m_List),
            end(m_List),
            [deviceId](DeviceListEntry^ e) {return e->Id == deviceId; });

        // if there's no match return.
        if (i == end(m_List)) 
        {
            return;
        }

        // Remove the last item from the list.
        MainPage::Current->NotifyUser("OnFx2Removed: " + deviceId + " removed", NotifyType::StatusMessage);
        m_List->RemoveAtEnd();
    }));
}

void DeviceList::OnFx2EnumerationComplete(DeviceWatcher ^ /* Sender */, Object ^ /* o */)
{
    MainPage::Current->Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler(
            [this]() 
    {
        MainPage::Current->NotifyUser("OnFx2EnumerationComplete", NotifyType::StatusMessage);

        DeviceList^ me = this;

        // Move all the unmatched elements to the end of the list
        auto i = std::remove_if(
            begin(m_List),
            end(m_List),
            [](DeviceListEntry^ e) { return e->Matched == false; });

        // Determine the number of unmatched entries
        auto unmatchedCount = end(m_List) - i;

        while (unmatchedCount > 0) 
        {
            m_List->RemoveAtEnd();
            unmatchedCount -= 1;
        }
    }));
}

void DeviceList::SuspendDeviceWatcher(Object^, SuspendingEventArgs^)
{
    if (WatcherStarted) 
    {
        m_WatcherSuspended = true;
        StopFx2Watcher();
    }
    else 
    {
        m_WatcherSuspended = false;
    }
}

void DeviceList::ResumeDeviceWatcher(Object^, Object^)
{
    if (m_WatcherSuspended) 
    {
        m_WatcherSuspended = false;
        StartFx2Watcher();
    }
}
