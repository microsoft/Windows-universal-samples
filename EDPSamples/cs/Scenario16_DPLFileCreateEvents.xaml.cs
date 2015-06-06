// Copyright (c) Microsoft. All rights reserved.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.ApplicationModel.Background;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Networking.PushNotifications;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using Windows.Security.EnterpriseData;
using System.Threading.Tasks;

namespace EdpSample
{
    /// <summary>
    /// Registering a background task and notification channel for raw notifications
    /// </summary>
    public sealed partial class Scenario16_DPLCreateFileEvents : Page
    {
        private MainPage rootPage = null;
        private const string SAMPLE_TASK_NAME = "DPLTask";
        private const string SAMPLE_TASK_ENTRY_POINT = "Tasks.EdpBackgroundTask";
    

        public Scenario16_DPLCreateFileEvents()
        {
             this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        void OutputToTextBox(String text)
        {
        }

        private void Register_Click(object sender, RoutedEventArgs e)
        {
            bool found = false;


            try
            {
                foreach (var iter in BackgroundTaskRegistration.AllTasks)
                {
                    IBackgroundTaskRegistration task = iter.Value;
                    if (task.Name == SAMPLE_TASK_NAME)
                    {
                        found = true;
                        AttachProgressAndCompletedHandlers(task);
                    }
                }

                if (!found)
                {
                    BackgroundTaskRegistration task = RegisterBackgroundTask();
                    if (task != null)
                    {
                        AttachProgressAndCompletedHandlers(task);
                    }
                }


                // Remove previous completion status from local settings.
                // Pass Enterprise ID to background task

                var settings = ApplicationData.Current.LocalSettings;
                settings.Values.Remove(SAMPLE_TASK_NAME);
                settings.Values.Add(SAMPLE_TASK_NAME, Scenario1.m_EnterpriseIdentity);

                rootPage.NotifyUser("Task registered", NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Registration error: " + ex.Message, NotifyType.ErrorMessage);
            }
       
        }

        private void UnRegister_Click(object sender, RoutedEventArgs e)
        {
            if (UnregisterBackgroundTask())
            {
                rootPage.NotifyUser("Task unregistered", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("No task is registered", NotifyType.ErrorMessage);
            }
        }


        private BackgroundTaskRegistration RegisterBackgroundTask()
        {
            BackgroundTaskRegistration task=null;

            try
            {

                task =  RegisterBackgroundTask(SAMPLE_TASK_ENTRY_POINT,
                                       SAMPLE_TASK_NAME,
                                       new SystemTrigger(SystemTriggerType.UserAway, false),
                                       null);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Registration error: " + ex.Message, NotifyType.ErrorMessage);
            }

            return task;
        }


        public static BackgroundTaskRegistration RegisterBackgroundTask(String taskEntryPoint, 
                                                                        String name, 
                                                                        IBackgroundTrigger trigger, 
                                                                        IBackgroundCondition condition)
        {
       
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

            BackgroundTaskRegistration task = builder.Register();

            return task;
        }

        private bool UnregisterBackgroundTask()
        {
            bool result = false;

            foreach (var iter in BackgroundTaskRegistration.AllTasks)
            {
                IBackgroundTaskRegistration task = iter.Value;
                if (task.Name == SAMPLE_TASK_NAME)
                {
                    task.Unregister(true);
                    result = true;
                }
            }
            return result;
        }

         private void AttachProgressAndCompletedHandlers(IBackgroundTaskRegistration task)
        {
            task.Progress += new BackgroundTaskProgressEventHandler(OnProgress);
            task.Completed += new BackgroundTaskCompletedEventHandler(OnCompleted);
        }

        private void OnProgress(IBackgroundTaskRegistration task, BackgroundTaskProgressEventArgs args)
        {
            
        }

        private void OnCompleted(IBackgroundTaskRegistration task, BackgroundTaskCompletedEventArgs args)
        {
            UpdateStatus();
        }

        private async void UpdateStatus()
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal,
           () =>
           {
               var settings = ApplicationData.Current.LocalSettings;
               rootPage.NotifyUser("CreateProtectedAndOpenAsync status: " + settings.Values[SAMPLE_TASK_NAME], NotifyType.StatusMessage);
           });
        }
    }
}
