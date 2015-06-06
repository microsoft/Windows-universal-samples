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

using SDKTemplate;
using System;
using Windows.Devices.Enumeration;
using Windows.Devices.Sensors;
using Windows.Foundation;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace ProximityCS
{
    public sealed partial class Scenario2_Polling : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private ProximitySensor sensor;
        private DeviceWatcher watcher;

        public Scenario2_Polling()
        {
            this.InitializeComponent();

            watcher = DeviceInformation.CreateWatcher(ProximitySensor.GetDeviceSelector());
            watcher.Added += OnProximitySensorAdded;
            watcher.Start();
        }

        /// <summary>
        /// Invoked when the device watcher finds a proximity sensor
        /// </summary>
        /// <param name="sender">The device watcher</param>
        /// <param name="device">Device information for the proximity sensor that was found</param>
        private async void OnProximitySensorAdded(DeviceWatcher sender, DeviceInformation device)
        {
            if (null == sensor)
            {
                ProximitySensor foundSensor = ProximitySensor.FromId(device.Id);
                if (null != foundSensor)
                {
                    // Optionally check the ProximitySensor.MaxDistanceInCentimeters/MinDistanceInCentimeters
                    // properties here. Refer to Scenario 1 for details.
                    sensor = foundSensor;
                }
                else
                {
                    await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        rootPage.NotifyUser("Could not get a proximity sensor from the device id", NotifyType.ErrorMessage);
                    });
                }
            }
        }

        /// <summary>
        /// This is the click handler for the 'GetData' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ScenarioGetData(object sender, RoutedEventArgs e)
        {
            if (null != sensor)
            {
                ProximitySensorReading reading = sensor.GetCurrentReading();
                if (null != reading)
                {
                    ScenarioOutput_DetectionState.Text = reading.IsDetected ? "Detected" : "Not detected";
                    ScenarioOutput_Timestamp.Text = reading.Timestamp.ToString("u");

                    // Show the detection distance, if available
                    if (null != reading.DistanceInMillimeters)
                    {
                        ScenarioOutput_DetectionDistance.Text = reading.DistanceInMillimeters.ToString();
                    }
                }
            }
            else
            {
                rootPage.NotifyUser("No proximity sensor found", NotifyType.ErrorMessage);
            }
        }
    }
}
