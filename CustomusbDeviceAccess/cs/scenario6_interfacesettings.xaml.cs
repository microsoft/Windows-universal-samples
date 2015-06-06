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

using System;
using System.Collections.Generic;
using System.Globalization;
using Windows.Devices.Usb;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;

namespace CustomUsbDeviceAccess
{
    /// <summary>
    /// Scenario will demonstrate how to change interface settings (also known as alternate interface setting).
    /// 
    /// This scenario can work for any device as long as it is "added" into the sample. For more information on how to add a 
    /// device to make it compatible with this scenario, please see Scenario1_DeviceConnect.
    /// </summary>
    public sealed partial class Scenario6_InterfaceSettings : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        private MainPage rootPage = MainPage.Current;

        public Scenario6_InterfaceSettings()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        ///
        /// We will enable/disable parts of the UI if the device doesn't support it.
        ///
        /// The list of interfaces settings will be hardcoded to be 0 or 1 because those settings on the
        /// defaultInterface of the SuperMutt are identical. Changing the setting will not effect the funcitonality
        /// of other scenarios.
        /// </summary>
        /// <param name="eventArgs">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs eventArgs)
        {
            // We will disable the scenario that is not supported by the device.
            // If no devices are connected, none of the scenarios will be shown and an error will be displayed
            Dictionary<DeviceType, UIElement> deviceScenarios = new Dictionary<DeviceType, UIElement>();
            deviceScenarios.Add(DeviceType.All, GenericScenario);

            Utilities.SetUpDeviceScenarios(deviceScenarios, DeviceScenarioContainer);

            // Enumerate all the interface settings of the default interface and add them to list for user to choose
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                var defaultInterface = EventHandlerForDevice.Current.Device.DefaultInterface;
                for (int settingNumber = 0; settingNumber < defaultInterface.InterfaceSettings.Count; settingNumber++)
                {
                    InterfaceSettingsToChoose.Items.Add(settingNumber.ToString(NumberFormatInfo.InvariantInfo));
                }

                // Default select the first interface setting because it's always going to be available
                InterfaceSettingsToChoose.SelectedIndex = 0;

                // Only allow setting of the interface settings for the SuperMutt device because it will not break the rest of the scenarios
                if (!Utilities.IsSuperMuttDevice(EventHandlerForDevice.Current.Device))
                {
                    ButtonSetSetting.IsEnabled = false;
                }
            }
        }

        /// <summary>
        /// Determines which item is clicked and what the interfaceSettingNumber that item corresponds to.
        /// Will not allow changing of the interface setting if not a SuperMutt. If it is a SuperMutt,
        /// only the first two interface settings can be used because the first two interface settings
        /// are identical with respect to the API.
        /// </summary>
        private void SetSuperMuttInterfaceSetting_Click(Object sender, RoutedEventArgs e)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                // Since we added the the settings in order, the index of the item clicked will give us the index of the
                // setting in UsbInterface.InterfaceSettings.
                // The number of interface settings is limited by the max value of byte because interface setting number size
                // is a byte (Usb Spec).
                SetInterfaceSetting((byte)InterfaceSettingsToChoose.SelectedIndex);
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

        private void GetInterfaceSetting_Click(Object sender, RoutedEventArgs e)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                GetInterfaceSetting();
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

        /// <summary>
        /// Selects the interface setting on the default interface.
        /// 
        /// The interfaceSetting is 0 based, where setting 0 is the default interfaceSetting.
        /// </summary>
        /// <param name="settingNumber"></param>
        private async void SetInterfaceSetting(Byte settingNumber)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                var interfaceSetting = EventHandlerForDevice.Current.Device.DefaultInterface.InterfaceSettings[settingNumber];

                await interfaceSetting.SelectSettingAsync();

                MainPage.Current.NotifyUser("Interface Setting is set to " + settingNumber, NotifyType.StatusMessage);
            }
        }

        /// <summary>
        /// Figures out which interface setting is currently selected by checking each interface setting even if the
        /// setting cannot be selected by this sample app.
        /// It will print out the selected interface setting number.
        /// <summary>
        private void GetInterfaceSetting()
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                var interfaceSettings = EventHandlerForDevice.Current.Device.DefaultInterface.InterfaceSettings;

                foreach (UsbInterfaceSetting interfaceSetting in interfaceSettings)
                {
                    if (interfaceSetting.Selected)
                    {
                        Byte interfaceSettingNumber = interfaceSetting.InterfaceDescriptor.AlternateSettingNumber;

                        MainPage.Current.NotifyUser("Current interface setting : " + interfaceSettingNumber.ToString("D", NumberFormatInfo.InvariantInfo), NotifyType.StatusMessage);

                        break;
                    }
                }
            }
        }
    }
}
