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

using BlockAndFilterTasks;
using System;
using Windows.ApplicationModel.Background;
using Windows.ApplicationModel.Calls.Background;
using Windows.ApplicationModel.CommunicationBlocking;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Register : Page
    {
        private MainPage rootPage = MainPage.Current;
        private bool AreTasksRegistered;

        public Scenario1_Register()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            UpdateBackgroundTaskUIState();
        }

        // Update the registration status text and the button states based on the background task's
        // registration state.
        private void UpdateBackgroundTaskUIState()
        {
            AreTasksRegistered = BackgroundTaskRegistration.AllTasks.Count > 0;
            if (AreTasksRegistered)
            {
                BackgroundTaskStatus.Text = "Background tasks are registered.";
                RegisterButton.Content = "Unregister background tasks";
            }
            else
            {
                BackgroundTaskStatus.Text = "Background tasks are not registered.";
                RegisterButton.Content = "Register background tasks";
            }
        }

        private void RegisterUnregister()
        {
            if (AreTasksRegistered)
            {
                // Unregister all background tasks.
                foreach (IBackgroundTaskRegistration task in BackgroundTaskRegistration.AllTasks.Values)
                {
                    task.Unregister(true);
                }

                rootPage.NotifyUser("Unregistered background tasks", NotifyType.StatusMessage);
            }
            else
            {
                // Register background tasks needed in order to be the active blocking and filtering app.

                // Mandatory: When a session begins, check if the app is the active blocking and filtering app.
                // If so, then set the list of blocked phone numbers.
                RegisterBackgroundTask(typeof(SessionConnectedTask), new SystemTrigger(SystemTriggerType.SessionConnected, false));

                // Mandatory: When the app is made the active blocking and filtering app, then
                // set the list of blocked phone numbers.
                RegisterBackgroundTask(typeof(ActivatedTask), new CommunicationBlockingAppSetAsActiveTrigger());

                // Mandatory: Run a task when the system blocks a call from the blocked numbers list.
                RegisterBackgroundTask(typeof(CallBlockedTask), new PhoneTrigger(PhoneTriggerType.CallBlocked, false));

                // Optional: Run a task when the system blocks a message from the blocked numbers list.
                RegisterBackgroundTask(typeof(MessageBlockedTask), new ChatMessageNotificationTrigger());

                // Optional: Run a task when a message is received, and evaluate whether the message should be blocked.
                // If this task is not registered, then only messages on the blocked numbers list will be blocked.
                RegisterBackgroundTask(typeof(MessageReceivedTask), new ChatMessageReceivedNotificationTrigger());

                rootPage.NotifyUser("Registered background tasks", NotifyType.StatusMessage);
            }
            UpdateBackgroundTaskUIState();
        }

        private void RegisterBackgroundTask(Type type, IBackgroundTrigger trigger)
        {
            // If the task is already registered. then don't register it again.
            foreach (IBackgroundTaskRegistration task in BackgroundTaskRegistration.AllTasks.Values)
            {
                if (task.Name == type.Name)
                {
                    return;
                }
            }

            var builder = new BackgroundTaskBuilder();
            builder.Name = type.Name;
            builder.TaskEntryPoint = type.FullName;
            builder.SetTrigger(trigger);
            BackgroundTaskRegistration taskRegistration = builder.Register();
        }

        private void CheckIsActive()
        {
            if (CommunicationBlockingAppManager.IsCurrentAppActiveBlockingApp)
            {
                rootPage.NotifyUser("App is the active block and filter app.", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("App is not the active block and filter app.", NotifyType.StatusMessage);
            }

        }

        private void SelectActiveApp()
        {
            // Select the active blocking app here.
            CommunicationBlockingAppManager.ShowCommunicationBlockingSettingsUI();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {

        }
    }
}
