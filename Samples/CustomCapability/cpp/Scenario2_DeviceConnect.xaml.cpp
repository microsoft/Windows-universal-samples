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
#include "MainPage.xaml.h"
#include "Scenario2_DeviceConnect.xaml.h"
#include "DeviceList.h"

using namespace Concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::Devices::Custom;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

DeviceConnect::DeviceConnect()
{
    InitializeComponent();
}

void DeviceConnect::OnNavigatedTo(NavigationEventArgs^ e)
{
    listSource->Source = DeviceList::Current->Fx2Devices;
    UpdateStartStopButtons();
}

void DeviceConnect::deviceConnectStart_Click(Object^ sender, RoutedEventArgs^ e)
{
    // The DeviceWatcher is in the DeviceList class.
    DeviceList::Current->StartFx2Watcher();
    UpdateStartStopButtons();
}


void DeviceConnect::deviceConnectStop_Click(Object^ sender, RoutedEventArgs^ e)
{
    DeviceList::Current->StopFx2Watcher();
    UpdateStartStopButtons();
}

void DeviceConnect::deviceConnectDevices_SelectionChanged(Object^ sender, SelectionChangedEventArgs^ e)
{
    auto entry = safe_cast<DeviceListEntry^>(deviceConnectDevices->SelectedItem);

    String^ currentlySelectedId = DeviceList::Current->GetSelectedDeviceId();
    String^ newlySelectedId = entry ? entry->Id : nullptr;

    if (currentlySelectedId != nullptr)
    {
        CloseFx2Device();
    }

    if (newlySelectedId != nullptr) 
    {
        OpenFx2DeviceAsync(newlySelectedId);
    }
}

void DeviceConnect::UpdateStartStopButtons()
{
    this->deviceConnectStart->IsEnabled = !DeviceList::Current->WatcherStarted;
    this->deviceConnectStop->IsEnabled = DeviceList::Current->WatcherStarted;
}

task<void> DeviceConnect::OpenFx2DeviceAsync(String^ id)
{
    return
        create_task(CustomDevice::FromIdAsync(id, DeviceAccessMode::ReadWrite, DeviceSharingMode::Exclusive)).
        then([this, id](task<CustomDevice^> result)
    {
        try
        {
            CustomDevice^ device = result.get();

            DeviceList::Current->SetSelectedDevice(id, device);

            rootPage->NotifyUser("Fx2 " + id + " opened", NotifyType::StatusMessage);
        }
        catch (Exception^ ex)
        {
            rootPage->NotifyUser("Error opening Fx2 device @" + id + ": " + ex->Message, NotifyType::ErrorMessage);
        }
    });
}

void DeviceConnect::CloseFx2Device()
{
    if (DeviceList::Current->GetSelectedDevice() != nullptr)
    {
        rootPage->NotifyUser("Fx2 " + DeviceList::Current->GetSelectedDeviceId() + " closed", NotifyType::StatusMessage);
        DeviceList::Current->ClearSelectedDevice();
    }
}
