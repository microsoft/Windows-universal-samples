//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#include "pch.h"
#include "Utilities.h"
#include "MainPage.xaml.h"

using namespace SDKTemplate;
using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;

/// <summary>
/// Displays the compatible scenarios and hides the non-compatible ones.
/// If there are no supported devices, the scenarioContainer will be hidden and an error message
/// will be displayed.
/// </summary>
/// <param name="scenarios">The key is the device type that the value, scenario, supports.</param>
/// <param name="scenarioContainer">The container that encompasses all the scenarios that are specific to devices</param>
void Utilities::SetUpDeviceScenarios(IMap<SDKTemplate::DeviceType, UIElement^>^ scenarios, UIElement^ scenarioContainer)
{
    UIElement^ supportedScenario = nullptr;

    if (EventHandlerForDevice::Current->IsDeviceConnected)
    {
        for (IKeyValuePair<DeviceType, UIElement^>^ deviceScenario : scenarios)
        {
            // Enable the scenario if it's generic or the device type matches
            if ((deviceScenario->Key == DeviceType::All)
                || (deviceScenario->Key == Utilities::GetDeviceType(EventHandlerForDevice::Current->Device)))
            {
                // Make the scenario visible in case other devices use the same scenario and collapsed it.
                deviceScenario->Value->Visibility = Visibility::Visible;

                supportedScenario = deviceScenario->Value;
            }
            else if (deviceScenario->Value != supportedScenario)    
            {
                // Don't hide the scenario if it is supported by the current device and is shared by other devices
                deviceScenario->Value->Visibility = Visibility::Collapsed;
            }
        }
    }

    if (supportedScenario == nullptr)
    {
        // Hide the container so that common elements shared across scenarios are also hidden
        scenarioContainer->Visibility = Visibility::Collapsed;

        NotifyDeviceNotConnected();
    }
}

/// <summary>
/// Will check if a device is currently connected (can be used). If there is no such device, this method will
/// notify the user with an error message.
/// </summary>
/// <returns>True if a notification was sent to the UI (No device is connected); False otherwise</returns>
void Utilities::NotifyDeviceNotConnected(void)
{
    MainPage::Current->NotifyUser("Device is not connected, please select a plugged in device to try the scenario again", NotifyType::ErrorMessage);
}

/// <summary>
/// Attempts to match the provided device with a well known device and returns the well known type
/// </summary>
/// <param name="device"></param>
/// <returns>Device type of the current connected device or DeviceType.None if there are no devices connected or is not recognized</returns>
DeviceType Utilities::GetDeviceType(Windows::Devices::HumanInterfaceDevice::HidDevice^ device)
{
    if (device != nullptr
        && device->VendorId == SuperMutt::Device::Vid
        && device->ProductId == SuperMutt::Device::Pid)
    {
            return DeviceType::SuperMutt;
    }
            
    return DeviceType::None;
}

bool Utilities::IsSuperMuttDevice(Windows::Devices::HumanInterfaceDevice::HidDevice^ device)
{
    return (GetDeviceType(device) == DeviceType::SuperMutt);
}