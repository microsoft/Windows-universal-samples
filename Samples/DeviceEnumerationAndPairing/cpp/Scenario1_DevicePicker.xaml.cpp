// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario1_DevicePicker.xaml.h"

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

Scenario1::Scenario1() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario1::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResultCollection = ref new Vector<DeviceInformationDisplay^>();

    selectorComboBox->ItemsSource = DeviceSelectorChoices::DevicePickerSelectors;
    selectorComboBox->SelectedIndex = 0;

    DataContext = this;
}

void Scenario1::OnNavigatedFrom(NavigationEventArgs^ e)
{
}

void Scenario1::PickSingleDeviceButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    ShowDevicePicker(
        true // pickSingle
        );
}

void Scenario1::ShowDevicePickerButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    ShowDevicePicker(
        false // pickSingle
        );
}

void Scenario1::ShowDevicePicker(bool pickSingle)
{
    showDevicePickerButton->IsEnabled = false;
    ResultCollection->Clear();

    devicePicker = ref new DevicePicker();
    
    // First get the device selector chosen by the UI.
    DeviceSelectorInfo^ deviceSelectorInfo = safe_cast<DeviceSelectorInfo^>(selectorComboBox->SelectedItem);

    if (nullptr == deviceSelectorInfo->Selector)
    {
        // If the a pre-canned device class selector was chosen, call the DeviceClass overload
        devicePicker->Filter->SupportedDeviceClasses->Append(deviceSelectorInfo->DeviceClassSelector);
    }
    else
    {
        // Use AQS string selector from dynamic call to a device api's GetDeviceSelector call
        devicePicker->Filter->SupportedDeviceSelectors->Append(deviceSelectorInfo->Selector);
    }

    rootPage->NotifyUser("Showing Device Picker", NotifyType::StatusMessage);

    // Calculate the position to show the picker (right below the buttons)
    GeneralTransform^ ge = pickSingleDeviceButton->TransformToVisual(nullptr);
    Point point = ge->TransformPoint(Point());
    Rect rect(point, Point(point.X + (float)pickSingleDeviceButton->ActualWidth, point.Y + (float)pickSingleDeviceButton->ActualHeight));
    
    if (pickSingle)
    {
        concurrency::create_task(devicePicker->PickSingleDeviceAsync(rect)).then(
            [this](DeviceInformation^ deviceInfo)
        {
            if (nullptr != deviceInfo)
            {
                ResultCollection->Append(ref new DeviceInformationDisplay(deviceInfo));
            }
            showDevicePickerButton->IsEnabled = true;
        }, concurrency::task_continuation_context::use_current());
    }
    else
    {
        devicePicker->DevicePickerDismissed += ref new TypedEventHandler<DevicePicker^, Object^>(
            [this](DevicePicker^ sender, Object^ obj)
        {
            // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
            rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
                [this]()
            {
                showDevicePickerButton->IsEnabled = true;
                rootPage->NotifyUser("Hiding Device Picker...", NotifyType::StatusMessage);
            }));
        });

        // Hook up handlers for the watcher events before starting the watcher
        devicePicker->DeviceSelected += ref new TypedEventHandler<DevicePicker^, DeviceSelectedEventArgs^>(
            [this](DevicePicker^ sender, DeviceSelectedEventArgs^ eventArgs)
        {
            // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
            rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
                [this, eventArgs]()
            {
                ResultCollection->Clear();
                ResultCollection->Append(ref new DeviceInformationDisplay(eventArgs->SelectedDevice));
            }));
        });

        devicePicker->Show(rect);
    }
}
