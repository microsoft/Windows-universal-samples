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
    // Enumerate all the LineDisplay devices and put them in our list box.
    create_task(DeviceInformation::FindAllAsync(LineDisplay::GetDeviceSelector(PosConnectionTypes::All)))
        .then([this](DeviceInformationCollection^ deviceInfoCollection)
    {
        for (DeviceInformation^ deviceInfo : deviceInfoCollection)
        {
            auto item = ref new ListBoxItem();
            item->Content = deviceInfo->Name;
            item->Tag = deviceInfo->Id;
            DisplaysListBox->Items->Append(item);
        }
    }, task_continuation_context::get_current_winrt_context());
}

void Scenario1_SelectDisplay::SelectButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    auto item = safe_cast<ListBoxItem^>(DisplaysListBox->SelectedValue);
    if (item != nullptr)
    {
        SelectButton->IsEnabled = false;

        auto name = safe_cast<String^>(item->Content);
        auto deviceId = safe_cast<String^>(item->Tag);
        create_task(ClaimedLineDisplay::FromIdAsync(deviceId))
            .then([this, name, deviceId](ClaimedLineDisplay^ lineDisplay)
        {
            if (lineDisplay != nullptr)
            {
                return create_task(lineDisplay->DefaultWindow->TryClearTextAsync())
                    .then([this, lineDisplay, name, deviceId](bool result)
                {
                    rootPage->NotifyUser("Selected: " + name, NotifyType::StatusMessage);

                    // Save this device ID for other scenarios.
                    rootPage->LineDisplayId = deviceId;

                    // Close the claimed line display.
                    delete lineDisplay;
                }, task_continuation_context::get_current_winrt_context());
            }
            else
            {
                rootPage->NotifyUser("Unable to claim the Line Display", NotifyType::ErrorMessage);
                return task_from_result();
            }
        }, task_continuation_context::get_current_winrt_context()).then([this]()
        {
            SelectButton->IsEnabled = true;
        }, task_continuation_context::get_current_winrt_context());
    }
}
