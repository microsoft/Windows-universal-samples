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

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace PedometerCS
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario4_BackgroundPedometer : Page
    {
        // Background task details
        public const string SampleBackgroundTaskName = "Scenario4_PedometerBackgroundTask";
        public const string SampleBackgroundTaskEntryPoint = "Tasks.PedometerBackgroundTask";

        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;
        bool backgroundTaskRegistered = false;
        int stepCount = 0;
        int stepGoal = 0;
        Pedometer sensor = null;
        // define a step goal of 50 steps from the current step count
        const int stepGoalOffset = 50;

        public Scenario4_BackgroundPedometer()
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
                if (task.Value.Name == Scenario4_BackgroundPedometer.SampleBackgroundTaskName)
                {
                    backgroundTaskRegistered = true;
                    break;
                }
            }
            UpdateUIAsync(backgroundTaskRegistered ? "Registered" : "Unregistered");
        }

        /// <summary>
        /// Invoked when 'Register Task' button is clicked. Attempts to find a default 
        /// Pedometer and registers a background task with a step goal of 50 steps.
        /// Adds a 
        /// </summary>
        private async void ScenarioRegisterTask_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            try
            {
                sensor = await Pedometer.GetDefaultAsync();
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
                    rootPage.NotifyUser("No pedometer sensors found", NotifyType.ErrorMessage);
                }
            }
            catch (UnauthorizedAccessException)
            {
                rootPage.NotifyUser("User has denied access to Pedometer history", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Registers the pedometer background task with a step goal of 50 steps.
        /// </summary>
        private void RegisterBackgroundTask()
        {
            // build the background task with the known entry points for Pedometer background task
            var builder = new BackgroundTaskBuilder()
            {
                Name = SampleBackgroundTaskName,
                TaskEntryPoint = SampleBackgroundTaskEntryPoint
            };

            // Get the current readings to find the current step counters
            var currentReadings = sensor.GetCurrentReadings();
            stepCount = 0;
            foreach (PedometerStepKind kind in Enum.GetValues(typeof(PedometerStepKind)))
            {
                PedometerReading reading;
                if (currentReadings.TryGetValue(kind, out reading))
                {
                    stepCount += reading.CumulativeSteps;
                }
            }

            // set a new step goal
            stepGoal = stepCount + Scenario4_BackgroundPedometer.stepGoalOffset;
            // create a Pedometer data threshold for that step goal
            var threshold = new PedometerDataThreshold(sensor, stepGoal);
            // create a sensor trigger using the above threshold
            var trigger = new SensorDataThresholdTrigger(threshold);

            builder.SetTrigger(trigger);

            BackgroundTaskRegistration task = builder.Register();
            task.Completed += new BackgroundTaskCompletedEventHandler(OnCompleted);

            backgroundTaskRegistered = true;
            UpdateUIAsync("Registered");
        }

        /// <summary>
        /// This is the event handler for background task completion.
        /// </summary>
        /// <param name="task">The task that is reporting completion.</param>
        /// <param name="args">The completion report arguments.</param>
        private void OnCompleted(IBackgroundTaskRegistration task, BackgroundTaskCompletedEventArgs args)
        {
            string status = "Completed and Unregistered";
            try
            {
                // check for the result from the background task
                args.CheckResult();
            }
            catch (Exception e)
            {
                status = e.Message;
            }
            // Pedometer background triggers are unique in a way as the background 
            // events are one-shot - Once the step-goal associated with the original 
            // task registration has been reached, that step count is a thing-of-past 
            // and shall not fire as the stepcount on that pedometer changes.
            // Unregister the background task or update the step goals here.
            task.Unregister(false);
            backgroundTaskRegistered = false;
            UpdateUIAsync(status);
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
                    backgroundTaskRegistered = false;
                    break;
                }
            }

            if (!backgroundTaskRegistered)
            {
                UpdateUIAsync("Unregistered");
            }
        }

        /// <summary>
        /// Updates the scenario UI.
        /// </summary>
        /// <param name="status">The status string to display.</param>
        async private void UpdateUIAsync(string status)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                ScenarioRegisterTaskButton.IsEnabled = !backgroundTaskRegistered;
                ScenarioUnregisterTaskButton.IsEnabled = backgroundTaskRegistered;
                ScenarioOutput_TaskRegistration.Text = status;

                var settings = ApplicationData.Current.LocalSettings;

                ScenarioOutput_CurrentCount.Text = stepCount.ToString();
                ScenarioOutput_StepGoal.Text = stepGoal.ToString();

                ScenarioOutput_UnknownCount.Text = "0";
                ScenarioOutput_WalkingCount.Text = "0";
                ScenarioOutput_RunningCount.Text = "0";

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
                    foreach (PedometerStepKind kind in Enum.GetValues(typeof(PedometerStepKind)))
                    {
                        if (settings.Values.TryGetValue(kind.ToString(), out value))
                        {
                            switch (kind)
                            {
                                case PedometerStepKind.Unknown:
                                    ScenarioOutput_UnknownCount.Text = value.ToString();
                                    break;
                                case PedometerStepKind.Walking:
                                    ScenarioOutput_WalkingCount.Text = value.ToString();
                                    break;
                                case PedometerStepKind.Running:
                                    ScenarioOutput_RunningCount.Text = value.ToString();
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                }
            });
        }
    }
}
