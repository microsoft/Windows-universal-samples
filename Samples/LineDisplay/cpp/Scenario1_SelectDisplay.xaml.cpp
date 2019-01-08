//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "Scenario1_SelectDisplay.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::PointOfService;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario1_SelectDisplay::Scenario1_SelectDisplay()
{
    InitializeComponent();
}

void Scenario1_SelectDisplay::OnNavigatedTo(NavigationEventArgs^ e)
{
    RestartWatcher();
}

void Scenario1_SelectDisplay::OnNavigatedFrom(NavigationEventArgs^ e)
{
    StopWatcher();
}

void Scenario1_SelectDisplay::RestartWatcher()
{
    StopWatcher();

    // Clear any old LineDisplay devices from the list box.
    DisplaysListBox->Items->Clear();

    // Enumerate the LineDisplay devices and put them in our list box.
    watcher = DeviceInformation::CreateWatcher(LineDisplay::GetDeviceSelector(PosConnectionTypes::All));
    watcher->Added += ref new TypedEventHandler<DeviceWatcher^, DeviceInformation^>(this, &Scenario1_SelectDisplay::Watcher_Added);
    watcher->EnumerationCompleted += ref new TypedEventHandler<DeviceWatcher^, Object^>(this, &Scenario1_SelectDisplay::Watcher_EnumerationCompleted);
    watcher->Start();
}

void Scenario1_SelectDisplay::StopWatcher()
{
    if (watcher && (watcher->Status == DeviceWatcherStatus::Started))
    {
        watcher->Stop();
    }
    watcher = nullptr;
}

void Scenario1_SelectDisplay::Watcher_Added(DeviceWatcher^ watcher, DeviceInformation^ deviceInfo)
{
    Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal,
        ref new Windows::UI::Core::DispatchedHandler([this, deviceInfo]
    {
        auto item = ref new ListBoxItem();
        item->Content = deviceInfo->Name;
        item->Tag = deviceInfo->Id;
        DisplaysListBox->Items->Append(item);
    }));
}

void Scenario1_SelectDisplay::Watcher_EnumerationCompleted(DeviceWatcher^ watcher, Object^ e)
{
    watcher->Stop();
}

void Scenario1_SelectDisplay::SelectButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    invoke_async_lambda([=]() -> task<void>
    {
        SelectButton->IsEnabled = false;

        auto item = safe_cast<ListBoxItem^>(DisplaysListBox->SelectedValue);

        auto name = safe_cast<String^>(item->Content);
        auto deviceId = safe_cast<String^>(item->Tag);
        ClaimedLineDisplay^ lineDisplay = co_await ClaimedLineDisplay::FromIdAsync(deviceId);
        if (lineDisplay != nullptr)
        {
            co_await lineDisplay->DefaultWindow->TryClearTextAsync();

            rootPage->NotifyUser("Selected: " + name, NotifyType::StatusMessage);

            // Save this device ID for other scenarios.
            rootPage->LineDisplayId = deviceId;

            // Close the claimed line display.
            delete lineDisplay;
        }
        else
        {
            rootPage->NotifyUser("Unable to claim the Line Display", NotifyType::ErrorMessage);
        }

        SelectButton->IsEnabled = true;
    });
}

void Scenario1_SelectDisplay::RefreshButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    RestartWatcher();
}