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

Scenario7_DeviceInformationKind::Scenario7_DeviceInformationKind()
{
    InitializeComponent();
}

void Scenario7_DeviceInformationKind::OnNavigatedTo(NavigationEventArgs^ e)
{
    resultsListView->ItemsSource = resultCollection;

    kindComboBox->ItemsSource = DeviceInformationKindChoices::Choices;
    kindComboBox->SelectedIndex = 0;
}

void Scenario7_DeviceInformationKind::OnNavigatedFrom(NavigationEventArgs^ e)
{
    StopWatchers(/* reset */ true);
}

void Scenario7_DeviceInformationKind::StartWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StartWatchers();
}

void Scenario7_DeviceInformationKind::StopWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StopWatchers();
}

void Scenario7_DeviceInformationKind::StartWatchers()
{
    startWatcherButton->IsEnabled = false;
    resultCollection->Clear();
    
    DeviceInformationKindChoice^ kindChoice = safe_cast<DeviceInformationKindChoice^>(kindComboBox->SelectedItem);

    // Create a watcher for each DeviceInformationKind selected by the user
    for (DeviceInformationKind deviceInfoKind : kindChoice->DeviceInformationKinds)
    {
        DeviceWatcher^ deviceWatcher = DeviceInformation::CreateWatcher(
            "", // AQS Filter string
            nullptr, // requested properties 
            deviceInfoKind);

        DeviceWatcherHelper^ deviceWatcherHelper = ref new DeviceWatcherHelper(resultCollection, Dispatcher);
        deviceWatcherHelper->UpdateStatus = false;
        deviceWatcherHelper->DeviceChanged += ref new TypedEventHandler<DeviceWatcher^, String^>(this, &Scenario7_DeviceInformationKind::OnDeviceListChanged);
        deviceWatcherHelpers->Append(deviceWatcherHelper);

        deviceWatcherHelper->StartWatcher(deviceWatcher);
    }
    
    stopWatcherButton->IsEnabled = true;
    stopWatcherButton->Focus(::FocusState::Keyboard);
}

void Scenario7_DeviceInformationKind::StopWatchers(bool reset)
{
    stopWatcherButton->IsEnabled = false;

    for (DeviceWatcherHelper^ deviceWatcherHelper : deviceWatcherHelpers)
    {
        deviceWatcherHelper->StopWatcher();
        if (reset)
        {
            deviceWatcherHelper->Reset();
        }
    }
    deviceWatcherHelpers->Clear();

    startWatcherButton->IsEnabled = true;
}

void Scenario7_DeviceInformationKind::OnDeviceListChanged(DeviceWatcher^ sender, String^ id)
{
    int watchersRunning = 0;

    // Count running watchers
    for (DeviceWatcherHelper^ deviceWatcherHelper : deviceWatcherHelpers)
    {
        if (deviceWatcherHelper->IsWatcherRunning())
        {
            watchersRunning++;
        }
    }

    String^ message = watchersRunning.ToString() + "/" + deviceWatcherHelpers->Size.ToString() + " watchers running. " +
        resultCollection->Size.ToString() + " devices found.";
    rootPage->NotifyUser(message, NotifyType::StatusMessage);
}

