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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using Windows.Devices.Enumeration;
using Windows.Devices.Sensors;
using Windows.Devices.Sensors.Custom;
using Windows.Foundation;
using System.Threading.Tasks;
using Windows.UI.Core;

namespace CustomSensors
{
    public sealed partial class Scenario2_Polling : Page
    {
        // The following ID is defined by vendors and is unique to a custom sensor type. Each custom sensor driver should define one unique ID.
        // 
        // The ID below is defined in the custom sensor driver sample available in the SDK. It identifies the custom sensor CO2 emulation sample driver.
        Guid GUIDCustomSensorDeviceVendorDefinedTypeID = new Guid("4025a865-638c-43aa-a688-98580961eeae");

        // A property key is defined by vendors for each datafield property a custom sensor driver exposes. Property keys are defined
        // per custom sensor driver and is unique to each custom sensor type.
        // 
        // The following property key represents the CO2 level as defined in the custom sensor CO2 emulation driver sample available in the WDK.
        // In this example only one key is defined, but other drivers may define more than one key by rev'ing up the property key index.
        const String CO2LevelKey = "{74879888-a3cc-45c6-9ea9-058838256433} 1";

        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private CustomSensor customSensor;
        private DeviceWatcher watcher;

        public Scenario2_Polling()
        {
            String customSensorSelector = "";

            this.InitializeComponent();

            customSensorSelector = CustomSensor.GetDeviceSelector(GUIDCustomSensorDeviceVendorDefinedTypeID);
            watcher = DeviceInformation.CreateWatcher(customSensorSelector);
            watcher.Added += OnCustomSensorAdded;
            watcher.Start();
        }

        /// <summary>
        /// Invoked when the device watcher finds a matching custom sensor device 
        /// </summary>
        /// <param name="watcher">device watcher</param>
        /// <param name="customSensorDevice">device information for the custom sensor that was found</param>
        public async void OnCustomSensorAdded(DeviceWatcher watcher, DeviceInformation customSensorDevice)
        {
            try
            {
                customSensor = await CustomSensor.FromIdAsync(customSensorDevice.Id);
                if (customSensor != null)
                {
                    CustomSensorReading reading = customSensor.GetCurrentReading();
                    if (!reading.Properties.ContainsKey(CO2LevelKey))
                    {
                        rootPage.NotifyUser("The found custom sensor doesn't provide CO2 reading", NotifyType.ErrorMessage);
                        customSensor = null;
                    }
                }
                else
                {
                    rootPage.NotifyUser("No custom sensor found", NotifyType.ErrorMessage);
                }
            }
            catch (Exception e)
            {
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    rootPage.NotifyUser("The user may have denied access to the custom sensor. Error: " + e.Message, NotifyType.ErrorMessage);
                });
            }
        }

        /// <summary>
        /// This is the dispatcher callback.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private void GetCO2Level(object sender, RoutedEventArgs e)
        {
            if (customSensor != null)
            {
                CustomSensorReading reading = customSensor.GetCurrentReading();

                string CO2LevelString = String.Format("{0,5:0.00}", reading.Properties[CO2LevelKey]);
                ScenarioOutputCO2Level.Text = CO2LevelString;
            }
            else
            {
                rootPage.NotifyUser("No custom sensor found", NotifyType.ErrorMessage);
            }
        }
    }
}
