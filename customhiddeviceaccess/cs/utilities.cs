//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

using System;
using System.Collections.Generic;
using Windows.Devices.HumanInterfaceDevice;
using Windows.UI.Xaml;
using SDKTemplate;

namespace CustomHidDeviceAccess
{
    public class Utilities
    {
        /// <summary>
        /// Displays the compatible scenarios and hides the non-compatible ones.
        /// If there are no supported devices, the scenarioContainer will be hidden and an error message
        /// will be displayed.
        /// </summary>
        /// <param name="scenarios">The key is the device type that the value, scenario, supports.</param>
        /// <param name="scenarioContainer">The container that encompasses all the scenarios that are specific to devices</param>
        public static void SetUpDeviceScenarios(Dictionary<DeviceType, UIElement> scenarios, UIElement scenarioContainer)
        {
            UIElement supportedScenario = null;

            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                foreach (KeyValuePair<DeviceType, UIElement> deviceScenario in scenarios)
                {
                    // Enable the scenario if it's generic or the device type matches
                    if ((deviceScenario.Key == DeviceType.Any)
                        || (deviceScenario.Key == Utilities.GetDeviceType(EventHandlerForDevice.Current.Device)))
                    {
                        // Make the scenario visible in case other devices use the same scenario and collapsed it.
                        deviceScenario.Value.Visibility = Visibility.Visible;

                        supportedScenario = deviceScenario.Value;
                    }
                    else if (deviceScenario.Value != supportedScenario)    // Don't hide the scenario if it is supported by the current device and is shared by other devices
                    {
                        deviceScenario.Value.Visibility = Visibility.Collapsed;
                    }
                }
            }

            if (supportedScenario == null)
            {
                // Hide the container so that common elements shared across scenarios are also hidden
                scenarioContainer.Visibility = Visibility.Collapsed;

                NotifyDeviceNotConnected();
            }
        }

        /// <summary>
        /// Device type of the device provided device.
        /// </summary>
        /// <param name="device"></param>
        /// <returns>The DeviceType of the device or DeviceType.None if there are no devices connected or is not recognized</returns>
        public static DeviceType GetDeviceType(HidDevice device)
        {
            if (device != null)
            {
                if (device != null
                    && device.VendorId == SuperMutt.Device.Vid
                    && device.ProductId == SuperMutt.Device.Pid)
                {
                    return DeviceType.SuperMutt;
                }

                return DeviceType.None;
            }

            return DeviceType.None;
        }

        public static Boolean IsSuperMuttDevice(HidDevice device)
        {
            return (GetDeviceType(device) == DeviceType.SuperMutt);
        }

        /// <summary>
        /// Prints an error message stating that device is not connected
        /// </summary>
        public static void NotifyDeviceNotConnected()
        {
            MainPage.Current.NotifyUser("Device is not connected, please select a plugged in device to try the scenario again", NotifyType.ErrorMessage);
        }
    }
}
