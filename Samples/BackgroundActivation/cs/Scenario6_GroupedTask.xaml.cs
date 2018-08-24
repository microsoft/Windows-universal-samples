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
using SDKTemplate;
using Windows.ApplicationModel.Background;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class GroupedBackgroundTask : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public GroupedBackgroundTask()
        {
            this.InitializeComponent();
        }

        BackgroundTaskRegistrationGroup group;

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            group = BackgroundTaskSample.GetTaskGroup(BackgroundTaskSample.BackgroundTaskGroupId, BackgroundTaskSample.BackgroundTaskGroupFriendlyName);

            foreach (var task in group.AllTasks)
            {
                if (task.Value.Name == BackgroundTaskSample.GroupedBackgroundTaskName)
                {
                    AttachProgressAndCompletedHandlers(task.Value);
                    BackgroundTaskSample.UpdateBackgroundTaskRegistrationStatus(BackgroundTaskSample.GroupedBackgroundTaskName, true);
                    break;
                }
            }

            UpdateUI();
        }

        /// <summary>
        /// Register a Grouped Background Task.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void RegisterGroupedBackgroundTask(object sender, RoutedEventArgs e)
        {
            var task = BackgroundTaskSample.RegisterBackgroundTask(null,
                                                                   BackgroundTaskSample.GroupedBackgroundTaskName,
                                                                   new SystemTrigger(SystemTriggerType.TimeZoneChange, false),
                                                                   null,
                                                                   group);
            AttachProgressAndCompletedHandlers(task);
            UpdateUI();
        }


        /// <summary>
        /// Unregister a Grouped Background Task.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void UnregisterGroupedTask(object sender, RoutedEventArgs e)
        {
            BackgroundTaskSample.UnregisterBackgroundTasks(BackgroundTaskSample.GroupedBackgroundTaskName, group);
            UpdateUI();
        }

        /// <summary>
        /// Unregisters all Background Tasks that are not in groups.
        /// This will not impact tasks registered with groups.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void UnregisterUngroupedTasks(object sender, RoutedEventArgs e)
        {
            foreach(var cur in BackgroundTaskRegistration.AllTasks)
            {
                cur.Value.Unregister(true);
                BackgroundTaskSample.UpdateBackgroundTaskRegistrationStatus(cur.Value.Name, false);
            }
        }

        /// <summary>
        /// Attach progress and completed handers to a background task.
        /// </summary>
        /// <param name="task">The task to attach progress and completed handlers to.</param>
        private void AttachProgressAndCompletedHandlers(IBackgroundTaskRegistration task)
        {
            task.Progress += new BackgroundTaskProgressEventHandler(OnProgress);
            task.Completed += new BackgroundTaskCompletedEventHandler(OnCompleted);
        }

        /// <summary>
        /// Handle background task progress.
        /// </summary>
        /// <param name="task">The task that is reporting progress.</param>
        /// <param name="e">Arguments of the progress report.</param>
        private void OnProgress(IBackgroundTaskRegistration task, BackgroundTaskProgressEventArgs args)
        {
            var ignored = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                var progress = "Progress: " + args.Progress + "%";
                BackgroundTaskSample.GroupedBackgroundTaskProgress = progress;
                UpdateUI();
            });
        }

        /// <summary>
        /// Handle background task completion.
        /// </summary>
        /// <param name="task">The task that is reporting completion.</param>
        /// <param name="e">Arguments of the completion report.</param>
        private void OnCompleted(IBackgroundTaskRegistration task, BackgroundTaskCompletedEventArgs args)
        {
            UpdateUI();
        }

        /// <summary>
        /// Update the scenario UI.
        /// </summary>
        private async void UpdateUI()
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal,
            () =>
            {
                RegisterButton.IsEnabled = !BackgroundTaskSample.GroupedBackgroundTaskRegistered;
                UnregisterGroupedButton.IsEnabled = BackgroundTaskSample.GroupedBackgroundTaskRegistered;
                Progress.Text = BackgroundTaskSample.GroupedBackgroundTaskProgress;
                Status.Text = BackgroundTaskSample.GetBackgroundTaskStatus(BackgroundTaskSample.GroupedBackgroundTaskName);
            });
        }
    }
}
