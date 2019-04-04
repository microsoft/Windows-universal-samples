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

Scenario2::Scenario2()
{
    InitializeComponent();
    deviceWatcherHelper = ref new DeviceWatcherHelper(resultCollection, Dispatcher);
}

void Scenario2::OnNavigatedTo(NavigationEventArgs^ e)
{
    resultsListView->ItemsSource = resultCollection;

    selectorComboBox->ItemsSource = DeviceSelectorChoices::DeviceWatcherSelectors;
    selectorComboBox->SelectedIndex = 0;
}

void Scenario2::OnNavigatedFrom(NavigationEventArgs^ e)
{
    deviceWatcherHelper->Reset();
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
    resultCollection->Clear();

    // First get the device selector chosen by the UI.
    DeviceSelectorInfo^ deviceSelectorInfo = safe_cast<DeviceSelectorInfo^>(selectorComboBox->SelectedItem);

    DeviceWatcher^ deviceWatcher;
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

    rootPage->NotifyUser("Starting Watcher...", NotifyType::StatusMessage);
    deviceWatcherHelper->StartWatcher(deviceWatcher);
    stopWatcherButton->IsEnabled = true;
}

void Scenario2::StopWatcher()
{
    stopWatcherButton->IsEnabled = false;
    deviceWatcherHelper->StopWatcher();
    startWatcherButton->IsEnabled = true;
}

