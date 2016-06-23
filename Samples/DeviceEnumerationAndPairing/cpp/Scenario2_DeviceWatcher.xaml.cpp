// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario2_DeviceWatcher.xaml.h"

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

Scenario2::Scenario2() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario2::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResultCollection = ref new Vector<DeviceInformationDisplay^>();

    selectorComboBox->ItemsSource = DeviceSelectorChoices::DeviceWatcherSelectors;
    selectorComboBox->SelectedIndex = 0;

    DataContext = this;
}

void Scenario2::OnNavigatedFrom(NavigationEventArgs^ e)
{
    StopWatcher();
}

void Scenario2::StartWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StartWatcher();
}

void Scenario2::StopWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StopWatcher();
}

void Scenario2::StartWatcher()
{
    startWatcherButton->IsEnabled = false;
    ResultCollection->Clear();
    
    // First get the device selector chosen by the UI.
    DeviceSelectorInfo^ deviceSelectorInfo = safe_cast<DeviceSelectorInfo^>(selectorComboBox->SelectedItem);

    if (nullptr == deviceSelectorInfo->Selector)
    {
        // If the a pre-canned device class selector was chosen, call the DeviceClass overload
        deviceWatcher = DeviceInformation::CreateWatcher(deviceSelectorInfo->DeviceClassSelector);
    }
    else if (deviceSelectorInfo->Kind == DeviceInformationKind::Unknown)
    {
        // Use AQS string selector from dynamic call to a device api's GetDeviceSelector call
        // Kind will be determined by the selector
        deviceWatcher = DeviceInformation::CreateWatcher(
            deviceSelectorInfo->Selector,
            nullptr // don't request additional properties for this sample
            );
    }
    else
    {
        // Kind is specified in the selector info
        deviceWatcher = DeviceInformation::CreateWatcher(
            deviceSelectorInfo->Selector,
            nullptr, // don't request additional properties for this sample
            deviceSelectorInfo->Kind);
    }

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
}

void Scenario2::StopWatcher()
{
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
}

