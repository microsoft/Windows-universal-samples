// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario6_CustomFilterAddedProps.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Devices::Enumeration;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario6::Scenario6() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario6::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResultCollection = ref new Vector<DeviceInformationDisplay^>();
    DataContext = this;
}

void Scenario6::OnNavigatedFrom(NavigationEventArgs^ e)
{
    StopWatcher();
}

void Scenario6::StartWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StartWatcher();
}

void Scenario6::StopWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StopWatcher();
}

void Scenario6::StartWatcher()
{
    aqsFilterTextBox->IsEnabled = false;
    startWatcherButton->IsEnabled = false;
    ResultCollection->Clear();

    // Request some additional properties.  In this saample, these extra properties are just used in the ResultsListViewTemplate. 
    // Take a look there in the XAML. Also look at the coverter used by the XAML GeneralPropertyValueConverter.  In general you just use
    // DeviceInformation.Properties["<property name>"] to get a property. e.g. DeviceInformation.Properties["System.Devices.InterfaceClassGuid"].
    Vector<String^>^ requestedProperties = ref new Vector<String^>();
    requestedProperties->Append("System.Devices.InterfaceClassGuid");
    requestedProperties->Append("System.ItemNameDisplay");

    // Use AQS string filter from the text box
    deviceWatcher = DeviceInformation::CreateWatcher(
        aqsFilterTextBox->Text,
        requestedProperties);

    // Hook up handlers for the watcher events before starting the watcher
    handlerAdded = ref new TypedEventHandler<DeviceWatcher^, DeviceInformation^>(
        [this](DeviceWatcher^ sender, DeviceInformation^ deviceInfo)
    {
        // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
        rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
            [this, deviceInfo]()
        {
            ResultCollection->Append(ref new DeviceInformationDisplay(deviceInfo));

            rootPage->NotifyUser(
                ResultCollection->Size.ToString() + " devices found.",
                NotifyType::StatusMessage);
        }));
    });
    handlerAddedToken = deviceWatcher->Added += handlerAdded;
    
    handlerUpdated = ref new TypedEventHandler<DeviceWatcher^, DeviceInformationUpdate^>(
        [this](DeviceWatcher^ sender, DeviceInformationUpdate^ deviceInfoUpdate)
    {
        // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
        rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
            [this, deviceInfoUpdate]()
        {
            // Find the corresponding updated DeviceInformation in the collection and pass the update object
            // to the Update method of the existing DeviceInformation. This automatically updates the object
            // for us.
            auto foundDeviceInfo = std::find_if(begin(ResultCollection), end(ResultCollection), [&](DeviceInformationDisplay^ di) { return (di->Id == deviceInfoUpdate->Id); });

            uint32 index = 0;
            if (foundDeviceInfo != end(ResultCollection))
            {
                (*foundDeviceInfo)->Update(deviceInfoUpdate);
            }
        }));
    });
    handlerUpdatedToken = deviceWatcher->Updated += handlerUpdated;

    handlerRemoved = ref new TypedEventHandler<DeviceWatcher^, DeviceInformationUpdate^>(
        [this](DeviceWatcher^ sender, DeviceInformationUpdate^ deviceInfoUpdate)
    {
        // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
        rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
            [this, deviceInfoUpdate]()
        {
            // Find the corresponding DeviceInformation in the collection and remove it
            auto foundDeviceInfo = std::find_if(begin(ResultCollection), end(ResultCollection), [&](DeviceInformationDisplay^ di){return (di->Id == deviceInfoUpdate->Id);});

            uint32 index = 0;
            if (foundDeviceInfo != end(ResultCollection) &&
                ResultCollection->IndexOf(*foundDeviceInfo, &index))
            {
                ResultCollection->RemoveAt(index);
            }

            rootPage->NotifyUser(
                ResultCollection->Size.ToString() + " devices found.",
                NotifyType::StatusMessage);
        }));
    });
    handlerRemovedToken = deviceWatcher->Removed += handlerRemoved;

    handlerEnumCompleted = ref new TypedEventHandler<DeviceWatcher^, Object^>(
        [this](DeviceWatcher^ sender, Object^ obj)
    {
        rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
            [this]()
        {
            rootPage->NotifyUser(
                ResultCollection->Size.ToString() + " devices found. Enumeration completed. Watching for updates...",
                NotifyType::StatusMessage);
        }));
    });
    handlerEnumCompletedToken = deviceWatcher->EnumerationCompleted += handlerEnumCompleted;

    handlerStopped = ref new TypedEventHandler<DeviceWatcher^, Object^>(
        [this](DeviceWatcher^ sender, Object^ obj)
    {
        rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
            [this, sender]()
        {
            String^ output = ResultCollection->Size.ToString();
            output += " devices found. Watcher ";
            output += (DeviceWatcherStatus::Aborted == sender->Status) ? "aborted" : "stopped";

            rootPage->NotifyUser(output, NotifyType::StatusMessage);
        }));
    });
    handlerStoppedAddedToken = deviceWatcher->Stopped += handlerStopped;

    rootPage->NotifyUser("Starting Watcher...", NotifyType::StatusMessage);
    deviceWatcher->Start();
    stopWatcherButton->IsEnabled = true;
    stopWatcherButton->Focus(Windows::UI::Xaml::FocusState::Keyboard);
    aqsFilterTextBox->IsEnabled = true;
}

void Scenario6::StopWatcher()
{
    aqsFilterTextBox->IsEnabled = false;
    stopWatcherButton->IsEnabled = false;

    if (nullptr != deviceWatcher)
    {
        // First unhook all event handlers except the stopped handler. This ensures our
        // event handlers don't get called after stop, as stop won't block for any "in flight" 
        // event handler calls.  We leave the stopped handler as it's guaranteed to only be called
        // once and we'll use it to know when the query is completely stopped. 
        deviceWatcher->Added -= handlerAddedToken;
        deviceWatcher->Updated -= handlerUpdatedToken;
        deviceWatcher->Removed -= handlerRemovedToken;
        deviceWatcher->EnumerationCompleted -= handlerEnumCompletedToken;

        if (DeviceWatcherStatus::Started == deviceWatcher->Status ||
            DeviceWatcherStatus::EnumerationCompleted == deviceWatcher->Status)
        {
            deviceWatcher->Stop();
        }
    }

    startWatcherButton->IsEnabled = true;
    startWatcherButton->Focus(Windows::UI::Xaml::FocusState::Keyboard);
    aqsFilterTextBox->IsEnabled = true;
}

