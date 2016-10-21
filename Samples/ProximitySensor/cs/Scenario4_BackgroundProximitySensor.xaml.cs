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
using Windows.ApplicationModel.Background;
using Windows.Devices.Enumeration;
using Windows.Devices.Sensors;
using Windows.Globalization.DateTimeFormatting;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Core;
using Windows.Storage;

namespace ProximityCS
{
    public sealed partial class Scenario4_BackgroundProximitySensor : Page
    {
        public const string SampleBackgroundTaskName = "ProximitySensorBackgroundTask";
        public const string SampleBackgroundTaskEntryPoint = "Tasks.ProximitySensorBackgroundTask";

        // Have a pointer to the main page to call methods in MainPage such as NotifyUser()
        private MainPage rootPage = MainPage.Current;
        private bool registered = false;
        private ProximitySensor sensor = null;
        private DeviceWatcher watcher = null;

        public Scenario4_BackgroundProximitySensor()
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
                sensor = ProximitySensor.FromId(device.Id);
                if (null == sensor)
                {
                    // failed to find the sensor corresponding to the id
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
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            foreach (var task in BackgroundTaskRegistration.AllTasks)
            {
                if (task.Value.Name == Scenario4_BackgroundProximitySensor.SampleBackgroundTaskName)
                {
                    registered = true;
                    break;
                }
            }
            UpdateUIAsync(registered ? "Registered" : "Unregistered");
        }

        /// <summary>
        /// Invoked when 'Register Task' button is clicked.
        /// Adds a 
        /// </summary>
        private async void ScenarioRegisterTask_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (sensor != null)
            {
                var status = await BackgroundExecutionManager.RequestAccessAsync();

                if ((BackgroundAccessStatus.AlwaysAllowed == status) ||
                    (BackgroundAccessStatus.AllowedSubjectToSystemPolicy == status))
                {
                    RegisterBackgroundTask();
                }
                else
                {
                    rootPage.NotifyUser("Background tasks may be disabled for this app", NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("No Proximity sensors found", NotifyType.ErrorMessage);
            }
        }


        /// <summary>
        /// Invoked when 'Unregister Task' button is clicked.
        /// </summary>
        private void ScenarioUnregisterTask_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            // Loop through all background tasks and unregister any with SampleBackgroundTaskName
            foreach (var cur in BackgroundTaskRegistration.AllTasks)
            {
                if (SampleBackgroundTaskName == cur.Value.Name)
                {
                    cur.Value.Unregister(true /*cancelTask*/);
                    registered = false;
                }
            }

            if (!registered)
            {
                UpdateUIAsync("Unregistered");
            }
        }


        /// <summary>
        /// Registers the background task.
        /// </summary>
        private void RegisterBackgroundTask()
        {
            var builder = new BackgroundTaskBuilder()
            {
                Name = SampleBackgroundTaskName,
                TaskEntryPoint = SampleBackgroundTaskEntryPoint
            };

            // create a Proximity data threshold
            var threshold = new ProximitySensorDataThreshold(sensor);
            // create a trigger using the threshold
            var trigger = new SensorDataThresholdTrigger(threshold);

            builder.SetTrigger(trigger);

            BackgroundTaskRegistration task = builder.Register();
            task.Completed += new BackgroundTaskCompletedEventHandler(OnCompleted);

            registered = true;
            UpdateUIAsync("Registered");
        }

        /// <summary>
        /// This is the event handler for background task completion.
        /// </summary>
        /// <param name="task">The task that is reporting completion.</param>
        /// <param name="args">The completion report arguments.</param>
        private void OnCompleted(IBackgroundTaskRegistration task, BackgroundTaskCompletedEventArgs args)
        {
            string status = "Completed";
            try
            {
                args.CheckResult();
            }
            catch (Exception e)
            {
                status = e.Message;
            }
            UpdateUIAsync(status);
        }


        /// <summary>
        /// Updates the scenario UI.
        /// </summary>
        /// <param name="status">The status string to display.</param>
        async private void UpdateUIAsync(string status)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                ScenarioRegisterTaskButton.IsEnabled = !registered;
                ScenarioUnregisterTaskButton.IsEnabled = registered;
                ScenarioOutput_TaskRegistration.Text = status;

                var settings = ApplicationData.Current.LocalSettings;

                Object value;

                if (settings.Values.TryGetValue("ReportCount", out value))
                {
                    ScenarioOutput_ReportCount.Text = value.ToString(); // stored as a string
                }
                if (settings.Values.TryGetValue("TaskStatus", out value))
                {
                    ScenarioOutput_TaskStatus.Text = value.ToString();
                }
                if (settings.Values.TryGetValue("LastTimestamp", out value))
                {
                    ScenarioOutput_LastTimestamp.Text = value.ToString();
                }
                if (settings.Values.TryGetValue("Detected", out value))
                {
                    ScenarioOutput_Detected.Text = value.ToString();
                }
            });
        }
    }
}
