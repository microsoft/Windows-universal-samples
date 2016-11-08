using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.ApplicationModel.Background;
using Windows.UI.Popups;
namespace FilePickerContracts
{
    class BGTaskHelper
    {
        public static async Task<BackgroundTaskRegistration> RegisterBackgroundTask()
        {
            //identify the trigger instance            
            IBackgroundTrigger trigger = null;
            trigger = new CachedFileUpdaterTrigger();

            await BackgroundExecutionManager.RequestAccessAsync();
            return await RegisterBackgroundTask(
                typeof(CachedFileUpdaterContract.UpdateFileTask).FullName,
                "Cached File Updater Task",
                trigger
                , null);
        }

        /// <summary>
        /// Register a background task with the specified taskEntryPoint, name, trigger,
        /// and condition (optional).
        /// </summary>
        /// <param name="taskEntryPoint">Task entry point for the background task.</param>
        /// <param name="name">A name for the background task.</param>
        /// <param name="trigger">The trigger for the background task.</param>
        /// <param name="condition">An optional conditional event that must be true for the task to fire.</param>
        /// <param name="cleanTask">An optional conditional parameter that removes the task upon registration call</param>
        public static async Task<BackgroundTaskRegistration> RegisterBackgroundTask(String taskEntryPoint, String name, IBackgroundTrigger trigger, IBackgroundCondition condition, bool cleanTask = false)
        {
            var registeredTask = FindTask(name) as BackgroundTaskRegistration;

            if (cleanTask && registeredTask != null)
            {
                // if task registered remove it
                UnregisterBackgroundTasks(name);
                registeredTask = null;
            }

            // if task is registered, nothing to do
            if (registeredTask != null)
                return registeredTask;

            var builder = new BackgroundTaskBuilder();

            builder.Name = name;
            builder.TaskEntryPoint = taskEntryPoint;
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

            BackgroundTaskRegistration task = null;
            Exception registrationException = null;

            task = builder.Register();

            if (registrationException != null)
            {
                Debug.WriteLine(registrationException);
                var dialog = new MessageDialog(registrationException.Message);
                await dialog.ShowAsync();
            }
            return task;
        }

        /// <summary>
        /// Unregister background tasks with specified name.
        /// </summary>
        /// <param name="name">Name of the background task to unregister.</param>
        public static void UnregisterBackgroundTasks(string name)
        {
            var task = FindTask(name);
            if (task != null)
                task.Unregister(true);
        }

        public static IBackgroundTaskRegistration FindTask(string name)
        {
            // Loop through all background tasks and find if a task was registered.
            // SampleBackgroundTaskWithConditionName.
            //
            foreach (var cur in BackgroundTaskRegistration.AllTasks)
            {
                if (cur.Value.Name == name)
                {
                    return cur.Value;
                }
            }
            return null;
        }

        /// <summary>
        /// Determine if task with given name requires background access.
        /// </summary>
        /// <param name="name">Name of background task to query background access requirement.</param>
        public static bool TaskRequiresBackgroundAccess(String name)
        {
            return true;
        }
    }
}
