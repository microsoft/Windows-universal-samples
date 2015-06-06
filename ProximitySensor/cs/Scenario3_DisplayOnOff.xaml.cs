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
    public sealed partial class Scenario3_DisplayOnOff : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private ProximitySensor sensor;
        private ProximitySensorDisplayOnOffController displayController;
        private DeviceWatcher watcher;

        public Scenario3_DisplayOnOff()
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
                if (null != displayController)
                {
                    displayController.Dispose(); // closes the controller
                    displayController = null;
                }
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
                // Acquires the display on/off controller for this proximity sensor.
                // This tells the system to use the sensor's IsDetected state to
                // turn the screen on or off.  If the display does not support this
                // feature, this code will do nothing.
                displayController = sensor.CreateDisplayOnOffController();

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
            // Releases the display on/off controller that is previously acquired.
            // This tells the system to stop using the sensor's IsDetected state to
            // turn the screen on or off.  If the display does not support this
            // feature, this code will do nothing.
            if (null != displayController)
            {
                displayController.Dispose(); // closes the controller
                displayController = null;
            }

            ScenarioEnableButton.IsEnabled = true;
            ScenarioDisableButton.IsEnabled = false;
        }
    }
}
