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
using Windows.ApplicationModel.Calls.Background;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Register : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario1_Register()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Set our initial UI state based on whether the background task is registered.
            IBackgroundTaskRegistration taskRegistration = MainPage.FindBackgroundTaskRegistration();
            UpdateBackgroundTaskUIState(taskRegistration != null);
        }

        // Update the registration status text and the button states based on the background task's
        // registration state.
        private void UpdateBackgroundTaskUIState(bool registered)
        {
            BackgroundTaskStatus.Text = registered ? "Registered" : "Unregistered";
            RegisterButton.IsEnabled = !registered;
            UnregisterButton.IsEnabled = registered;
        }

        private async void RegisterTask()
        {
            var access = await BackgroundExecutionManager.RequestAccessAsync();
            if (access == BackgroundAccessStatus.AlwaysAllowed ||
                access == BackgroundAccessStatus.AllowedSubjectToSystemPolicy)
            {
                // Create a new background task builder.
                BackgroundTaskBuilder taskBuilder = new BackgroundTaskBuilder();

                // Create a new Phone Trigger.
                PhoneTrigger trigger = new PhoneTrigger(PhoneTriggerType.CallOriginDataRequest, false);

                // Associate the phone trigger with the background task builder.
                taskBuilder.SetTrigger(trigger);

                // Specify the background task to run when the trigger fires.
                taskBuilder.TaskEntryPoint = MainPage.BackgroundTaskEntryPoint;

                // Name the background task.
                taskBuilder.Name = MainPage.BackgroundTaskName;

                // Register the background task.
                BackgroundTaskRegistration taskRegistration = taskBuilder.Register();

                UpdateBackgroundTaskUIState(true);
                rootPage.NotifyUser("Registered Caller ID background task", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("Background agent access denied", NotifyType.ErrorMessage);
            }
        }


        // Handle request to unregister the background task
        private void UnregisterTask()
        {
            IBackgroundTaskRegistration taskRegistration = MainPage.FindBackgroundTaskRegistration();
            if (taskRegistration != null)
            {
                taskRegistration.Unregister(true);
                rootPage.NotifyUser("Unregistered Caller ID background task.", NotifyType.StatusMessage);
            }
            UpdateBackgroundTaskUIState(false);
        }
    }
}
