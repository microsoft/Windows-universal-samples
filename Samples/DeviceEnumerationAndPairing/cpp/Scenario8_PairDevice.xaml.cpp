// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario8_PairDevice.xaml.h"

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

Scenario8_PairDevice::Scenario8_PairDevice()
{
    InitializeComponent();
    deviceWatcherHelper = ref new DeviceWatcherHelper(resultCollection, Dispatcher);
    deviceWatcherHelper->DeviceChanged += ref new TypedEventHandler<DeviceWatcher^, String^>(this, &Scenario8_PairDevice::OnDeviceListChanged);
}

void Scenario8_PairDevice::OnNavigatedTo(NavigationEventArgs^ e)
{
    resultsListView->ItemsSource = resultCollection;
    
    selectorComboBox->ItemsSource = DeviceSelectorChoices::PairingSelectors;
    selectorComboBox->SelectedIndex = 0;
}

void Scenario8_PairDevice::OnNavigatedFrom(NavigationEventArgs^ e)
{
    deviceWatcherHelper->Reset();
}

void Scenario8_PairDevice::StartWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StartWatcher();
}

void Scenario8_PairDevice::StopWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StopWatcher();
}

void Scenario8_PairDevice::StartWatcher()
{
    startWatcherButton->IsEnabled = false;
    resultCollection->Clear();

    // Get the device selector chosen by the UI then add additional constraints for devices that 
    // can be paired or are already paired.
    DeviceSelectorInfo^ deviceSelectorInfo = safe_cast<DeviceSelectorInfo^>(selectorComboBox->SelectedItem);
    String^ selector = "(" + deviceSelectorInfo->Selector + ")" + " AND (System.Devices.Aep.CanPair:=System.StructuredQueryType.Boolean#True OR System.Devices.Aep.IsPaired:=System.StructuredQueryType.Boolean#True)";

    DeviceWatcher^ deviceWatcher;
    if (deviceSelectorInfo->Kind == DeviceInformationKind::Unknown)
    {
        // Kind will be determined by the selector
        deviceWatcher = DeviceInformation::CreateWatcher(
            selector,
            nullptr // don't request additional properties for this sample
            );
    }
    else
    {
        // Kind is specified in the selector info
        deviceWatcher = DeviceInformation::CreateWatcher(
            selector,
            nullptr, // don't request additional properties for this sample
            deviceSelectorInfo->Kind);
    }

    rootPage->NotifyUser("Starting Watcher...", NotifyType::StatusMessage);
    deviceWatcherHelper->StartWatcher(deviceWatcher);
    stopWatcherButton->IsEnabled = true;
}

void Scenario8_PairDevice::OnDeviceListChanged(DeviceWatcher^ sender, String^ id)
{
    // If the item being updated is currently "selected", then update the pairing buttons
    DeviceInformationDisplay^ selectedDeviceInfoDisp = safe_cast<DeviceInformationDisplay^>(resultsListView->SelectedItem);
    if ((selectedDeviceInfoDisp != nullptr) && (selectedDeviceInfoDisp->Id == id))
    {
        UpdatePairingButtons();
    }
}


void Scenario8_PairDevice::StopWatcher()
{
    stopWatcherButton->IsEnabled = false;

    deviceWatcherHelper->StopWatcher();

    startWatcherButton->IsEnabled = true;
}

void Scenario8_PairDevice::PairButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Gray out the pair button and results view while pairing is in progress.
    resultsListView->IsEnabled = false;
    pairButton->IsEnabled = false;
    rootPage->NotifyUser("Pairing started. Please wait...", NotifyType::StatusMessage);

    DeviceInformationDisplay^ deviceInfoDisp = safe_cast<DeviceInformationDisplay^>(resultsListView->SelectedItem);

    concurrency::create_task(deviceInfoDisp->DeviceInfo->Pairing->PairAsync()).then(
        [this](DevicePairingResult^ pairingResult)
    {
        rootPage->NotifyUser(
            "Pairing result = " + (pairingResult->Status == DevicePairingResultStatus::Paired ? "Paired" : "NotPaired"),
            pairingResult->Status == DevicePairingResultStatus::Paired ? NotifyType::StatusMessage : NotifyType::ErrorMessage);

        UpdatePairingButtons();
        resultsListView->IsEnabled = true;
    }, concurrency::task_continuation_context::use_current());
}

void Scenario8_PairDevice::UnpairButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Gray out the unpair button and results view while unpairing is in progress.
    resultsListView->IsEnabled = false;
    unpairButton->IsEnabled = false;
    rootPage->NotifyUser("Unpairing started. Please wait...", NotifyType::StatusMessage);

    DeviceInformationDisplay^ deviceInfoDisp = safe_cast<DeviceInformationDisplay^>(resultsListView->SelectedItem);

    concurrency::create_task(deviceInfoDisp->DeviceInfo->Pairing->UnpairAsync()).then(
        [this](DeviceUnpairingResult^ unpairingResult)
    {
        rootPage->NotifyUser(
            "Unpairing result = " + (unpairingResult->Status == DeviceUnpairingResultStatus::Unpaired ? "Unpaired" : "Failed"),
            unpairingResult->Status == DeviceUnpairingResultStatus::Unpaired ? NotifyType::StatusMessage : NotifyType::ErrorMessage);

        UpdatePairingButtons();
        resultsListView->IsEnabled = true;
    }, concurrency::task_continuation_context::use_current());
}

void Scenario8_PairDevice::ResultsListView_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
    UpdatePairingButtons();
}

void Scenario8_PairDevice::UpdatePairingButtons()
{
    DeviceInformationDisplay^ deviceInfoDisp = safe_cast<DeviceInformationDisplay^>(resultsListView->SelectedItem);

    if (nullptr != deviceInfoDisp &&
        deviceInfoDisp->DeviceInfo->Pairing->CanPair &&
        !deviceInfoDisp->DeviceInfo->Pairing->IsPaired)
    {
        pairButton->IsEnabled = true;
    }
    else
    {
        pairButton->IsEnabled = false;
    }

    if (nullptr != deviceInfoDisp &&
        deviceInfoDisp->DeviceInfo->Pairing->IsPaired)
    {
        unpairButton->IsEnabled = true;
    }
    else
    {
        unpairButton->IsEnabled = false;
    }
}


