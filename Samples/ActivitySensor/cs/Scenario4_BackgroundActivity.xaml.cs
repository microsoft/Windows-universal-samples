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
using Windows.ApplicationModel.Background;
using Windows.Devices.Enumeration;
using Windows.Devices.Sensors;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario4_BackgroundActivity : Page
    {
        public const string SampleBackgroundTaskName = "ActivitySensorBackgroundTask";
        public const string SampleBackgroundTaskEntryPoint = "Tasks.ActivitySensorBackgroundTask";

        // Common class ID for activity sensors
        Guid ActivitySensorClassId = new Guid("9D9E0118-1807-4F2E-96E4-2CE57142E196");

        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;
        bool registered = false;

        public Scenario4_BackgroundActivity()
        {
            this.InitializeComponent();
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
                if (task.Value.Name == Scenario4_BackgroundActivity.SampleBackgroundTaskName)
                {
                    registered = true;
                    break;
                }
            }

            UpdateUIAsync(registered ? "Registered" : "Unregistered");
        }

        /// <summary>
        /// This is the click handler for the 'Register Task' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void ScenarioRegisterTask(object sender, RoutedEventArgs e)
        {
            // Determine if we can access activity sensors
            var deviceAccessInfo = DeviceAccessInformation.CreateFromDeviceClassId(ActivitySensorClassId);
            if (deviceAccessInfo.CurrentStatus == DeviceAccessStatus.Allowed)
            {
                // Determine if an activity sensor is present
                // This can also be done using Windows::Devices::Enumeration::DeviceInformation::FindAllAsync
                var activitySensor = await ActivitySensor.GetDefaultAsync();
                if (true||activitySensor != null)
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
                    rootPage.NotifyUser("No activity sensors found", NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("Access denied to activity sensors", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// This is the click handler for the 'Unregister Task' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ScenarioUnregisterTask(object sender, RoutedEventArgs e)
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
        /// Registers the background task.
        /// </summary>
        private void RegisterBackgroundTask()
        {
            var builder = new BackgroundTaskBuilder()
            {
                Name = SampleBackgroundTaskName,
                TaskEntryPoint = SampleBackgroundTaskEntryPoint
            };

            const UInt32 reportIntervalMs = 3000; // 3 seconds
            var trigger = new ActivitySensorTrigger(reportIntervalMs);

            trigger.SubscribedActivities.Add(ActivityType.Walking);
            trigger.SubscribedActivities.Add(ActivityType.Running);
            trigger.SubscribedActivities.Add(ActivityType.Biking);

            builder.SetTrigger(trigger);

            BackgroundTaskRegistration task = builder.Register();
            task.Completed += new BackgroundTaskCompletedEventHandler(OnCompleted);

            registered = true;
            UpdateUIAsync("Registered");
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

                if (settings.Values.ContainsKey("ReportCount"))
                {
                    ScenarioOutput_ReportCount.Text = settings.Values["ReportCount"].ToString(); // stored as a string
                }
                if (settings.Values.ContainsKey("TaskStatus"))
                {
                    ScenarioOutput_TaskStatus.Text = settings.Values["TaskStatus"].ToString(); // stored as a string
                }
                if (settings.Values.ContainsKey("LastActivity"))
                {
                    ScenarioOutput_LastActivity.Text = (string)settings.Values["LastActivity"]; // stored as a string
                }
                if (settings.Values.ContainsKey("LastConfidence"))
                {
                    ScenarioOutput_LastConfidence.Text = (string)settings.Values["LastConfidence"]; // stored as a string
                }
                if (settings.Values.ContainsKey("LastTimestamp"))
                {
                    ScenarioOutput_LastTimestamp.Text = settings.Values["LastTimestamp"].ToString(); // stored as a string
                }
            });
        }
    }
}
