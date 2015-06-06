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
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Windows.Devices.Sensors;
using Windows.Devices.Sensors.Custom;
using Windows.Foundation;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;


namespace CustomSensors
{
    public sealed partial class Scenario1_DataEvents : Page
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
        private uint desiredReportInterval;
        private DeviceWatcher watcher;

        public Scenario1_DataEvents()
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
                    else
                    {
                        // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
                        // This value will be used later to activate the sensor.
                        // In the case below, we defined a 200ms report interval as being suitable for the purpose of this app.
                        UInt32 minReportInterval = customSensor.MinimumReportInterval;
                        desiredReportInterval = minReportInterval > 200 ? minReportInterval : 200;
                    }

                }
                else
                {
                    rootPage.NotifyUser("No custom sensor found", NotifyType.ErrorMessage);
                }
            }
            catch(Exception e)
            {
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    rootPage.NotifyUser("The user may have denied access to the custom sensor. Error: " + e.Message, NotifyType.ErrorMessage);
                });
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
                Window.Current.VisibilityChanged -= new WindowVisibilityChangedEventHandler(VisibilityChanged);
                customSensor.ReadingChanged -= new TypedEventHandler<CustomSensor, CustomSensorReadingChangedEventArgs>(ReadingChanged);

                // Restore the default report interval to release resources while the sensor is not in use
                customSensor.ReportInterval = 0;
            }

            base.OnNavigatingFrom(e);
        }

        /// <summary>
        /// This is the event handler for VisibilityChanged events. You would register for these notifications
        /// if handling sensor data when the app is not visible could cause unintended actions in the app.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e">
        /// Event data that can be examined for the current visibility state.
        /// </param>
        private void VisibilityChanged(object sender, VisibilityChangedEventArgs e)
        {
            if (ScenarioDisableButton.IsEnabled)
            {
                if (e.Visible)
                {
                    // Re-enable sensor input (no need to restore the desired reportInterval... it is restored for us upon app resume)
                    customSensor.ReadingChanged += new TypedEventHandler<CustomSensor, CustomSensorReadingChangedEventArgs>(ReadingChanged);
                }
                else
                {
                    // Disable sensor input (no need to restore the default reportInterval... resources will be released upon app suspension)
                    customSensor.ReadingChanged -= new TypedEventHandler<CustomSensor, CustomSensorReadingChangedEventArgs>(ReadingChanged);
                }
            }
        }

        /// <summary>
        /// This is the event handler for ReadingChanged events.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void ReadingChanged(object sender, CustomSensorReadingChangedEventArgs e)
        {
            CustomSensorReading reading = e.Reading;

            string CO2LevelString = String.Format("{0,5:0.00}", reading.Properties[CO2LevelKey]);
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                ScenarioOutputCO2Level.Text = CO2LevelString;
            });
        }

        /// <summary>
        /// This is the click handler for the 'Enable' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ScenarioEnable(object sender, RoutedEventArgs e)
        {
            if (customSensor != null)
            {
                // Establish the report interval
                customSensor.ReportInterval = desiredReportInterval;

                Window.Current.VisibilityChanged += new WindowVisibilityChangedEventHandler(VisibilityChanged);
                customSensor.ReadingChanged += new TypedEventHandler<CustomSensor, CustomSensorReadingChangedEventArgs>(ReadingChanged);

                ScenarioEnableButton.IsEnabled = false;
                ScenarioDisableButton.IsEnabled = true;
            }
            else
            {
                rootPage.NotifyUser("No custom sensor found", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// This is the click handler for the 'Disable' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ScenarioDisable(object sender, RoutedEventArgs e)
        {
            Window.Current.VisibilityChanged -= new WindowVisibilityChangedEventHandler(VisibilityChanged);
            customSensor.ReadingChanged -= new TypedEventHandler<CustomSensor, CustomSensorReadingChangedEventArgs>(ReadingChanged);

            // Restore the default report interval to release resources while the sensor is not in use
            customSensor.ReportInterval = 0;

            ScenarioEnableButton.IsEnabled = true;
            ScenarioDisableButton.IsEnabled = false;
        }
    }
}
