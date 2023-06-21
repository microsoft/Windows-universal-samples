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
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Windows.Devices.Sensors;
using Windows.Foundation;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario3_Choosing : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private DeviceWatcher watcher = null;

        public Scenario3_Choosing()
        {
            InitializeComponent();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            if (watcher != null)
            {
                watcher.Stop();
            }
        }

        private async void ScenarioPick_Click(object sender, RoutedEventArgs e)
        {
            DevicePicker picker = new DevicePicker();
            picker.Appearance.Title = "Select a presence sensor from which to read data";
            picker.Filter.SupportedDeviceSelectors.Add(HumanPresenceSensor.GetDeviceSelector());

            // Calculate the position of the button next to which we will place the picker.
            GeneralTransform transform = ScenarioPickButton.TransformToVisual(null);
            Rect rect = transform.TransformBounds(new Rect(0.0, 0.0, ScenarioPickButton.ActualWidth, ScenarioPickButton.ActualHeight));

            DeviceInformation deviceInfo = await picker.PickSingleDeviceAsync(rect);
            if (deviceInfo != null)
            {
                await ReadOneReadingFromSensorAsync(deviceInfo);
            }
        }

        /// <summary>
        /// Invoked when the device watcher finds a presence sensor
        /// </summary>
        /// <param name="sender">The device watcher</param>
        /// <param name="device">Device information for the presence sensor that was found</param>
        private async void OnHumanPresenceSensorAdded(DeviceWatcher sender, DeviceInformation deviceInfo)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                // The app can optionally ignore sensors which do not satisfy desired criteria.
                // This sample does not apply any filter. See the DeviceEnumerationAndPairing sample
                // for more advanced ways of presenting a list of devices.

                ListBoxItem item = new ListBoxItem() { Content = deviceInfo.Name, Tag = deviceInfo };
                SensorsList.Items.Add(item);
            });
        }

        /// <summary>
        /// Invoked when a presence sensor is removed from the system
        /// </summary>
        /// <param name="sender">The device watcher</param>
        /// <param name="device">Device information for the presence sensor that was removed</param>
        private async void OnHumanPresenceSensorRemoved(DeviceWatcher sender, DeviceInformationUpdate update)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                // Look for the sensor that was removed from the system and erase it from the ListBox.
                string removedId = update.Id;

                foreach (ListBoxItem item in SensorsList.Items)
                {
                    DeviceInformation itemDeviceInfo = (DeviceInformation)item.Tag;
                    if (itemDeviceInfo.Id == removedId)
                    {
                        SensorsList.Items.Remove(item);
                        break;
                    }
                }
            });
        }

        /// <summary>
        /// Invoked when a presence sensor's properties change
        /// </summary>
        /// <param name="sender">The device watcher</param>
        /// <param name="device">Device information for the presence sensor that changed</param>
        private async void OnHumanPresenceSensorUpdated(DeviceWatcher sender, DeviceInformationUpdate update)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                // Look for the sensor that was updated.
                string updatedId = update.Id;

                foreach (ListBoxItem item in SensorsList.Items)
                {
                    DeviceInformation itemDeviceInfo = (DeviceInformation)item.Tag;
                    if (itemDeviceInfo.Id == updatedId)
                    {
                        // Update the DeviceInformation for the sensor that updated.
                        itemDeviceInfo.Update(update);
                        // Refresh the device name, in case it changed.
                        item.Content = itemDeviceInfo.Name;
                        break;
                    }
                }
            });
        }

        private async void OnHumanPresenceSensorEnumerationCompleted(DeviceWatcher sender, object e)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                rootPage.NotifyUser("All sensors found. Monitoring for changes...", NotifyType.StatusMessage);
            });
        }

        private void ScenarioStart_Click(object sender, RoutedEventArgs e)
        {
            SensorsList.Items.Clear();

            watcher = DeviceInformation.CreateWatcher(HumanPresenceSensor.GetDeviceSelector());
            watcher.Added += OnHumanPresenceSensorAdded;
            watcher.Removed += OnHumanPresenceSensorRemoved;
            watcher.Updated += OnHumanPresenceSensorUpdated;
            watcher.EnumerationCompleted += OnHumanPresenceSensorEnumerationCompleted;
            watcher.Start();

            rootPage.NotifyUser("Looking for sensors...", NotifyType.StatusMessage);

            ScenarioStartButton.IsEnabled = false;
            ScenarioStopButton.IsEnabled = true;
        }

        private void ScenarioStop_Click(object sender, RoutedEventArgs e)
        {
            watcher.Stop();
            watcher = null;

            rootPage.NotifyUser("", NotifyType.StatusMessage);

            ScenarioStartButton.IsEnabled = true;
            ScenarioStopButton.IsEnabled = false;
        }

        private async void ScenarioReadSelected_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            ListBoxItem item = (ListBoxItem)SensorsList.SelectedItem;
            if (item == null)
            {
                rootPage.NotifyUser("No sensor is selected.", NotifyType.ErrorMessage);
                return;
            }

            DeviceInformation selectedInfo = (DeviceInformation)item.Tag;
            await ReadOneReadingFromSensorAsync(selectedInfo);
        }

        // Obtains a sensor described by the DeviceInformation and reads one reading from it.
        private async Task ReadOneReadingFromSensorAsync(DeviceInformation deviceInfo)
        {
            HumanPresenceSensor sensor;
            try
            {
                sensor = await HumanPresenceSensor.FromIdAsync(deviceInfo.Id);
            }
            catch (UnauthorizedAccessException)
            {
                // The user denied this app presence sensor access.
                sensor = null;
            }

            EngagementStateTextBlock.Text = "No data";
            DetectionStateTextBlock.Text = "No data";
            TimestampTextBlock.Text = "No data";
            DetectionDistanceTextBlock.Text = "No data";

            if (sensor != null)
            {
                HumanPresenceSensorReading reading;
                try
                {
                    reading = sensor.GetCurrentReading();
                }
                catch (UnauthorizedAccessException)
                {
                    // The user denied this app presence sensor access.
                    reading = null;
                }

                if (reading != null)
                {
                    if (sensor.IsEngagementSupported)
                    {
                        EngagementStateTextBlock.Text = reading.Engagement.ToString();
                    }
                    else
                    {
                        EngagementStateTextBlock.Text = "Not Supported";
                    }

                    if (sensor.IsPresenceSupported)
                    {
                        DetectionStateTextBlock.Text = reading.Presence.ToString();
                    }
                    else
                    {
                        DetectionStateTextBlock.Text = "Not Supported";
                    }

                    // Show the detection distance, if available
                    DetectionDistanceTextBlock.Text = reading.DistanceInMillimeters == null ? "Unavailable" : reading.DistanceInMillimeters.ToString();

                    TimestampTextBlock.Text = reading.Timestamp.ToString("u");
                }
            }
            else
            {
                rootPage.NotifyUser($"Could not access sensor {deviceInfo.Name}", NotifyType.ErrorMessage);
            }
        }
    }
}
