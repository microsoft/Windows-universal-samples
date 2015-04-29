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
    public sealed partial class Scenario1_DataEvents : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private ProximitySensor sensor;
        private DeviceWatcher watcher;

        public Scenario1_DataEvents()
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
                    if (null != foundSensor.MaxDistanceInMillimeters)
                    {
                        // Check if this is the sensor that matches our ranges.

                        // TODO: Customize these values to your application's needs.
                        // Here, we are looking for a sensor that can detect close objects
                        // up to 3cm away, so we check the upper bound of the detection range.
                        const uint distanceInMillimetersLValue = 30; // 3 cm
                        const uint distanceInMillimetersRValue = 50; // 5 cm

                        if (foundSensor.MaxDistanceInMillimeters >= distanceInMillimetersLValue &&
                            foundSensor.MaxDistanceInMillimeters <= distanceInMillimetersRValue)
                        {
                            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                            {
                                rootPage.NotifyUser("Found a proximity sensor that meets the detection range", NotifyType.StatusMessage);
                            });
                        }
                        else
                        {
                            foundSensor = null;
                        }
                    }
                    else
                    {
                        await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                        {
                            rootPage.NotifyUser("Proximity sensor does not report detection ranges, using it anyway", NotifyType.StatusMessage);
                        });
                    }

                    if (null != foundSensor)
                    {
                        sensor = foundSensor;
                    }
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
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached. The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            ScenarioEnableButton.IsEnabled = true;
            ScenarioDisableButton.IsEnabled = false;
        }

        /// <summary>
        /// Invoked immediately before the Page is unloaded and is no longer the current source of a parent Frame.
        /// </summary>
        /// <param name="e">
        /// Event data that can be examined by overriding code. The event data is representative
        /// of the navigation that will unload the current Page unless canceled. The
        /// navigation can potentially be canceled by setting Cancel.
        /// </param>
        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            if (ScenarioDisableButton.IsEnabled)
            {
                sensor.ReadingChanged -= new TypedEventHandler<ProximitySensor, ProximitySensorReadingChangedEventArgs>(ReadingChanged);
            }

            base.OnNavigatingFrom(e);
        }

        /// <summary>
        /// This is the click handler for the 'Enable' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ScenarioEnable(object sender, RoutedEventArgs e)
        {
            if (null != sensor)
            {
                sensor.ReadingChanged += new TypedEventHandler<ProximitySensor, ProximitySensorReadingChangedEventArgs>(ReadingChanged);

                ScenarioEnableButton.IsEnabled = false;
                ScenarioDisableButton.IsEnabled = true;
            }
            else
            {
                rootPage.NotifyUser("No proximity sensors found", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// This is the click handler for the 'Disable' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ScenarioDisable(object sender, RoutedEventArgs e)
        {
            sensor.ReadingChanged -= new TypedEventHandler<ProximitySensor, ProximitySensorReadingChangedEventArgs>(ReadingChanged);

            ScenarioEnableButton.IsEnabled = true;
            ScenarioDisableButton.IsEnabled = false;
        }

        /// <summary>
        /// This is the event handler for ReadingChanged events.
        /// </summary>
        /// <param name="sender">The proximity sensor that sent the reading change</param>
        /// <param name="e"></param>
        async private void ReadingChanged(ProximitySensor sender, ProximitySensorReadingChangedEventArgs e)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                ProximitySensorReading reading = e.Reading;
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
            });
        }
    }
}
