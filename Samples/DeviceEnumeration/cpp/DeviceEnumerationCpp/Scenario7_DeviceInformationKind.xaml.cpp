// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario7_DeviceInformationKind.xaml.h"

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

Scenario7::Scenario7() : rootPage(MainPage::Current)
{
    InitializeComponent();
    
    deviceWatchers = ref new Vector<DeviceWatcher^>();
}

void Scenario7::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResultCollection = ref new Vector<DeviceInformationDisplay^>();

    kindComboBox->ItemsSource = DeviceInformationKindChoices::Choices;
    kindComboBox->SelectedIndex = 0;

    DataContext = this;
}

void Scenario7::OnNavigatedFrom(NavigationEventArgs^ e)
{
    StopWatcher();
}

void Scenario7::StartWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StartWatcher();
}

void Scenario7::StopWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StopWatcher();
}

void Scenario7::StartWatcher()
{
    startWatcherButton->IsEnabled = false;
    ResultCollection->Clear();
    deviceWatchers->Clear();
    
    // Create all the event handlers

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

    handlerRemoved = ref new TypedEventHandler<DeviceWatcher^, DeviceInformationUpdate^>(
        [this](DeviceWatcher^ sender, DeviceInformationUpdate^ deviceInfoUpdate)
    {
        // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
        rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
            [this, deviceInfoUpdate]()
        {
            // Find the corresponding DeviceInformation in the collection and remove it
            auto foundDeviceInfo = std::find_if(begin(ResultCollection), end(ResultCollection), [&](DeviceInformationDisplay^ di) { return (di->Id == deviceInfoUpdate->Id); });

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
    
    DeviceInformationKindChoice^ kindChoice = safe_cast<DeviceInformationKindChoice^>(kindComboBox->SelectedItem);

    // Allocate arrays to hold event registration tokens
    handlerAddedTokens = ref new Array<EventRegistrationToken>(kindChoice->DeviceInformationKinds->Size);
    handlerUpdatedTokens = ref new Array<EventRegistrationToken>(kindChoice->DeviceInformationKinds->Size);
    handlerRemovedTokens = ref new Array<EventRegistrationToken>(kindChoice->DeviceInformationKinds->Size);
    handlerEnumCompletedTokens = ref new Array<EventRegistrationToken>(kindChoice->DeviceInformationKinds->Size);
    handlerStoppedTokens = ref new Array<EventRegistrationToken>(kindChoice->DeviceInformationKinds->Size);

    // Create a watcher for each DeviceInformationKind selected by the user
    for (uint32 i = 0; i < kindChoice->DeviceInformationKinds->Size; i++)
    {
        DeviceWatcher^ deviceWatcher = DeviceInformation::CreateWatcher(
            "", // AQS Filter string
            nullptr, // requested properties 
            kindChoice->DeviceInformationKinds->GetAt(i));

        deviceWatchers->Append(deviceWatcher);

        // Hook up handlers for the watcher events before starting the watcher

        handlerAddedTokens->set(i, deviceWatcher->Added += handlerAdded);
        handlerUpdatedTokens->set(i, deviceWatcher->Updated += handlerUpdated);
        handlerRemovedTokens->set(i, deviceWatcher->Removed += handlerRemoved);
        handlerEnumCompletedTokens->set(i, deviceWatcher->EnumerationCompleted += handlerEnumCompleted);
        handlerStoppedTokens->set(i, deviceWatcher->Stopped += handlerStopped);

        deviceWatcher->Start();
    }
    
    stopWatcherButton->IsEnabled = true;
    stopWatcherButton->Focus(Windows::UI::Xaml::FocusState::Keyboard);
}

void Scenario7::StopWatcher()
{
    stopWatcherButton->IsEnabled = false;

    for (uint32 i = 0; i < deviceWatchers->Size; i++)
    {
        // First unhook all event handlers except the stopped handler. This ensures our
        // event handlers don't get called after stop, as stop won't block for any "in flight" 
        // event handler calls.  We leave the stopped handler as it's guaranteed to only be called
        // once and we'll use it to know when the query is completely stopped. 
        deviceWatchers->GetAt(i)->Added -= handlerAddedTokens[i];
        deviceWatchers->GetAt(i)->Updated -= handlerUpdatedTokens[i];
        deviceWatchers->GetAt(i)->Removed -= handlerRemovedTokens[i];
        deviceWatchers->GetAt(i)->EnumerationCompleted -= handlerEnumCompletedTokens[i];

        if (DeviceWatcherStatus::Started == deviceWatchers->GetAt(i)->Status ||
            DeviceWatcherStatus::EnumerationCompleted == deviceWatchers->GetAt(i)->Status)
        {
            deviceWatchers->GetAt(i)->Stop();
        }
    }

    startWatcherButton->IsEnabled = true;
}

