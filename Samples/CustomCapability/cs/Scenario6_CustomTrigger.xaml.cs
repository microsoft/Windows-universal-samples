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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class CustomTrigger
    {
        MainPage rootPage = MainPage.Current;

        BackgroundTaskRegistration taskRegistration;

        const string customTriggerTaskName = "Osrusbfx2Task.ConnectedTask";
        const string guidCustomSystemEventId_OsrUsbFx2 = "{C7BDD245-5CDA-4BBD-B68D-B5E36F7911A3}";

        public CustomTrigger()
        {
            this.InitializeComponent();
            UpdateUI();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            UnregisterTask();
        }

        void UpdateUI()
        {
            RegisterCustomTrigger.IsEnabled = (taskRegistration == null);
            UnregisterCustomTrigger.IsEnabled = (taskRegistration != null);
        }

        void UnregisterTask()
        {
            if (taskRegistration != null)
            {
                taskRegistration.Unregister(true);
                taskRegistration = null;
            }
        }

        void RegisterCustomTrigger_Click(Object sender, RoutedEventArgs e)
        {
            if (IsFx2CustomTriggerTaskRegistered())
            {
                rootPage.NotifyUser("Osrusbfx2.ConnectedTask background task has already been registered", NotifyType.ErrorMessage);
            }

            // Create a new background task builder.
            BackgroundTaskBuilder taskBuilder = new BackgroundTaskBuilder();

            // Create a new OEM trigger.
            CustomSystemEventTrigger fx2Trigger = new CustomSystemEventTrigger(
                guidCustomSystemEventId_OsrUsbFx2, // Trigger Qualifier
                CustomSystemEventTriggerRecurrence.Once); // One-shot trigger

            // Associate the fx2Trigger trigger with the background task builder.
            taskBuilder.SetTrigger(fx2Trigger);

            // Specify the background task to run when the trigger fires.
            taskBuilder.TaskEntryPoint = customTriggerTaskName;

            // Name the background task.
            taskBuilder.Name = customTriggerTaskName;

            try
            {
                // Register the background task.
                taskRegistration = taskBuilder.Register();
            }
            catch (Exception ex) {
                rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
                return;
            }

            UpdateUI();
        }

        bool IsFx2CustomTriggerTaskRegistered()
        {
            foreach (var task in BackgroundTaskRegistration.AllTasks)
            {
                string taskName = task.Value.Name;
                if (taskName == customTriggerTaskName)
                {
                    return true;
                }
            }

            return false;
        }

        void UnregisterCustomTrigger_Click(Object sender, RoutedEventArgs e)
        {
            UnregisterTask();
            UpdateUI();
        }
    }
}
