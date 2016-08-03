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
using System.Threading.Tasks;
using Windows.Foundation.Collections;
using Windows.UI.Xaml.Controls;
using Windows.ApplicationModel.Activation;
using Windows.ApplicationModel.Background;
using Windows.Storage;
using Windows.UI.Xaml;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Background Activation";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Background Task", ClassType=typeof(SampleBackgroundTask)},
            new Scenario() { Title="Background Task with Condition", ClassType=typeof(SampleBackgroundTaskWithCondition)},
            new Scenario() { Title="Servicing Complete Task", ClassType=typeof(ServicingCompleteTask)},
            new Scenario() { Title="Background Task with Time Trigger", ClassType=typeof(TimeTriggeredTask) },
            new Scenario() { Title="Background Task with Application Trigger", ClassType=typeof(ApplicationTriggerTask) }
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}

namespace SDKTemplate
{
    class BackgroundTaskSample
    {
        public const string SampleBackgroundTaskName = "SampleBackgroundTask";
        public static string SampleBackgroundTaskProgress = "";
        public static bool SampleBackgroundTaskRegistered = false;

        public const string SampleBackgroundTaskWithConditionName = "SampleBackgroundTaskWithCondition";
        public static string SampleBackgroundTaskWithConditionProgress = "";
        public static bool SampleBackgroundTaskWithConditionRegistered = false;

        public const string ServicingCompleteTaskName = "ServicingCompleteTask";
        public static string ServicingCompleteTaskProgress = "";
        public static bool ServicingCompleteTaskRegistered = false;

        public const string TimeTriggeredTaskName = "TimeTriggeredTask";
        public static string TimeTriggeredTaskProgress = "";
        public static bool TimeTriggeredTaskRegistered = false;

        public const string ApplicationTriggerTaskName = "ApplicationTriggerTask";
        public static string ApplicationTriggerTaskProgress = "";
        public static string ApplicationTriggerTaskResult = "";
        public static bool ApplicationTriggerTaskRegistered = false;

        // These strings are manipulated by multiple threads, so we must
        // use a thread-safe container.
        public static PropertySet TaskStatuses = new PropertySet();

        /// <summary>
        /// Register a background task with the specified taskEntryPoint, name, trigger,
        /// and condition (optional).
        /// </summary>
        /// <param name="taskEntryPoint">Task entry point for the background task.</param>
        /// <param name="name">A name for the background task.</param>
        /// <param name="trigger">The trigger for the background task.</param>
        /// <param name="condition">An optional conditional event that must be true for the task to fire.</param>
        public static BackgroundTaskRegistration RegisterBackgroundTask(String taskEntryPoint, String name, IBackgroundTrigger trigger, IBackgroundCondition condition)
        {
            if (TaskRequiresBackgroundAccess(name))
            {
                // If the user denies access, the task will not run.
                var requestTask = BackgroundExecutionManager.RequestAccessAsync();
            }

            var builder = new BackgroundTaskBuilder();

            builder.Name = name;

            if (taskEntryPoint != null)
            {
                // If you leave the TaskEntryPoint at its default value, then the task runs
                // inside the main process from OnBackgroundActivated rather than as a separate process.
                builder.TaskEntryPoint = taskEntryPoint;  
            }
            
            builder.SetTrigger(trigger);

            if (condition != null)
            {
                builder.AddCondition(condition);

                //
                // If the condition changes while the background task is executing then it will
                // be canceled.
                //
                builder.CancelOnConditionLoss = true;
            }

            BackgroundTaskRegistration task = builder.Register();

            UpdateBackgroundTaskRegistrationStatus(name, true);

            //
            // Remove previous completion status.
            //
            TaskStatuses.Remove(name);

            return task;
        }

        /// <summary>
        /// Unregister background tasks with specified name.
        /// </summary>
        /// <param name="name">Name of the background task to unregister.</param>
        public static void UnregisterBackgroundTasks(String name)
        {
            //
            // Loop through all background tasks and unregister any with SampleBackgroundTaskName or
            // SampleBackgroundTaskWithConditionName.
            //
            foreach (var cur in BackgroundTaskRegistration.AllTasks)
            {
                if (cur.Value.Name == name)
                {
                    cur.Value.Unregister(true);
                }
            }

            UpdateBackgroundTaskRegistrationStatus(name, false);
        }

        /// <summary>
        /// Store the registration status of a background task with a given name.
        /// </summary>
        /// <param name="name">Name of background task to store registration status for.</param>
        /// <param name="registered">TRUE if registered, FALSE if unregistered.</param>
        public static void UpdateBackgroundTaskRegistrationStatus(String name, bool registered)
        {
            switch (name)
            {
                case SampleBackgroundTaskName:
                    SampleBackgroundTaskRegistered = registered;
                    break;
                case SampleBackgroundTaskWithConditionName:
                    SampleBackgroundTaskWithConditionRegistered = registered;
                    break;
                case ServicingCompleteTaskName:
                    ServicingCompleteTaskRegistered = registered;
                    break;
                case TimeTriggeredTaskName:
                    TimeTriggeredTaskRegistered = registered;
                    break;
                case ApplicationTriggerTaskName:
                    ApplicationTriggerTaskRegistered = registered;
                    break;
            }
        }

        /// <summary>
        /// Get the registration / completion status of the background task with
        /// given name.
        /// </summary>
        /// <param name="name">Name of background task to retreive registration status.</param>
        public static String GetBackgroundTaskStatus(String name)
        {
            var registered = false;
            switch (name)
            {
                case SampleBackgroundTaskName:
                    registered = SampleBackgroundTaskRegistered;
                    break;
                case SampleBackgroundTaskWithConditionName:
                    registered = SampleBackgroundTaskWithConditionRegistered;
                    break;
                case ServicingCompleteTaskName:
                    registered = ServicingCompleteTaskRegistered;
                    break;
                case TimeTriggeredTaskName:
                    registered = TimeTriggeredTaskRegistered;
                    break;
                case ApplicationTriggerTaskName:
                    registered = ApplicationTriggerTaskRegistered;
                    break;
            }

            var status = registered ? "Registered" : "Unregistered";

            object taskStatus;
            if (TaskStatuses.TryGetValue(name, out taskStatus))
            {
                status += " - " + taskStatus.ToString();
            }

            return status;
        }

        /// <summary>
        /// Determine if task with given name requires background access.
        /// </summary>
        /// <param name="name">Name of background task to query background access requirement.</param>
        public static bool TaskRequiresBackgroundAccess(String name)
        {
            if ((name == TimeTriggeredTaskName) ||
                (name == ApplicationTriggerTaskName))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}


namespace SDKTemplate
{
    sealed partial class App : Application
    {
        /// <summary>
        /// Override the Application.OnBackgroundActivated method to handle background activation in 
        /// the main process. This entry point is used when BackgroundTaskBuilder.TaskEntryPoint is 
        /// not set during background task registration.
        /// </summary>
        /// <param name="args"></param>
        protected override void OnBackgroundActivated(BackgroundActivatedEventArgs args)
        {
            BackgroundActivity.Start(args.TaskInstance);
        }
    }
}