// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario4_Snapshot.xaml.h"

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

Scenario4::Scenario4() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario4::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResultCollection = ref new Vector<DeviceInformationDisplay^>();

    selectorComboBox->ItemsSource = DeviceSelectorChoices::FindAllAsyncSelectors;
    selectorComboBox->SelectedIndex = 0;

    DataContext = this;
}

void Scenario4::FindButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    DeviceSelectorInfo^ deviceSelectorInfo;
    IAsyncOperation<DeviceInformationCollection^>^ asyncOp;

    findButton->IsEnabled = false;
    ResultCollection->Clear();

    // First get the device selector chosen by the UI.
    deviceSelectorInfo = safe_cast<DeviceSelectorInfo^>(selectorComboBox->SelectedItem);

    rootPage->NotifyUser("FindAllAsync operation started...", NotifyType::StatusMessage);

    if (nullptr == deviceSelectorInfo->Selector)
    {
        // If the a pre-canned device class selector was chosen, call the DeviceClass overload
        asyncOp = DeviceInformation::FindAllAsync(deviceSelectorInfo->DeviceClassSelector);
    }
    else
    {
        // Use AQS string selector from dynamic call to a device api's GetDeviceSelector call
        asyncOp = DeviceInformation::FindAllAsync(
            deviceSelectorInfo->Selector,
            nullptr // don't request additional properties for this sample
            );
    }

    // Notice below we'll use the default context insted of the current (UI thread) to reduce load
    // on the UI thread. Updates to UI bound elements will be done with a low priority dispatch. 
    concurrency::create_task(asyncOp).then(
        [this](DeviceInformationCollection^ deviceInfoCollection)
    {
        rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
            [this, deviceInfoCollection]()
        {
            rootPage->NotifyUser(
                "FindAllAsync operation completed. " + deviceInfoCollection->Size.ToString() + " devices found.",
                NotifyType::StatusMessage);
        }));

        std::for_each(begin(deviceInfoCollection), end(deviceInfoCollection), [&](DeviceInformation^ deviceInfo)
        {
            rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
                [this, deviceInfo]()
            {
                ResultCollection->Append(ref new DeviceInformationDisplay(deviceInfo));
            }));
        });

        rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
            [this]()
        {
            findButton->IsEnabled = true;
        }));
    }, concurrency::task_continuation_context::use_default());
}

