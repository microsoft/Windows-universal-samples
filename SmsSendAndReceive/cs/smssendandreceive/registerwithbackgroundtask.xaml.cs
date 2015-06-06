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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using System.Collections.Generic;
using Windows.UI.Core;
using Windows.Storage;
using Windows.ApplicationModel.Background;
using Windows.Devices.Sms;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SmsSendAndReceive
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class RegisterWithBackgroundTask : Page
    {
        private MainPage rootPage;
        private const string BackgroundTaskEntryPoint = "SmsBackgroundTask.SampleSmsBackgroundTask";
        private const string BackgroundTaskName = "SampleSmsBackgroundTask";
        private CoreDispatcher sampleDispatcher;

        public RegisterWithBackgroundTask()
        {
            this.InitializeComponent();

            // Get dispatcher for dispatching updates to the UI thread.
            sampleDispatcher = Window.Current.CoreWindow.Dispatcher;

            RegisterButton.IsEnabled = true;
            UnregisterButton.IsEnabled = false;

            try
            {
                // Initialize state-based registration of currently registered background tasks.
                InitializeRegisteredSmsBackgroundTasks();
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }

        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        // Update the registration status text and the button states based on the background task's
        // registration state.
        private void UpdateBackgroundTaskUIState(bool Registered)
        {
            if (Registered)
            {
                BackgroundTaskStatus.Text = "Registered";
                RegisterButton.IsEnabled = false;
                UnregisterButton.IsEnabled = true;
            }
            else
            {
                BackgroundTaskStatus.Text = "Unregistered";
                RegisterButton.IsEnabled = true;
                UnregisterButton.IsEnabled = false;
            }
        }

        // Initialize state based on currently registered background tasks
        public void InitializeRegisteredSmsBackgroundTasks()
        {
            try
            {
                //
                // Initialize UI elements based on currently registered background tasks
                // and associate background task completed event handler with each background task.
                //
                UpdateBackgroundTaskUIState(false);

                foreach (var item in BackgroundTaskRegistration.AllTasks)
                {
                    IBackgroundTaskRegistration task = item.Value;
                    if (task.Name == BackgroundTaskName)
                    {
                        UpdateBackgroundTaskUIState(true);
                        task.Completed += new BackgroundTaskCompletedEventHandler(OnCompleted);
                    }
                }
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }
        }


        private void Register_Click(object sender, RoutedEventArgs e)
        {
            // Check to see that a filter action type is set
            if (AcceptImmediatelyButton.IsChecked == false && DropButton.IsChecked == false && PeekButton.IsChecked == false && AcceptButton.IsChecked == false)
            {
                rootPage.NotifyUser("Please set a filter action type", NotifyType.ErrorMessage);
                return;
            }

            // Check to see that a message type is set
            if (TextButton.IsChecked == false && AppButton.IsChecked == false && WapButton.IsChecked == false && BroadcastButton.IsChecked == false && VoicemailButton.IsChecked == false)
            {
                rootPage.NotifyUser("Please set a message type", NotifyType.ErrorMessage);
                return;
            }

            try
            {
                // Set filter action type
                SmsMessageType messageType = SmsMessageType.Text; // set as Text as default
                if (TextButton.IsChecked == true)
                {
                    messageType = SmsMessageType.Text;
                }
                else if (AppButton.IsChecked == true)
                {
                    messageType = SmsMessageType.App;
                }
                else if (WapButton.IsChecked == true)
                {
                    messageType = SmsMessageType.Wap;
                }
                else if (BroadcastButton.IsChecked == true)
                {
                    messageType = SmsMessageType.Broadcast;
                }
                else if (VoicemailButton.IsChecked == true)
                {
                    messageType = SmsMessageType.Voicemail;
                }

                // Create new filter rule (individual)
                SmsFilterRule filter = new SmsFilterRule(messageType);

                // Set filter action type
                SmsFilterActionType actionType = SmsFilterActionType.Accept; // set as Accept as default
                if (AcceptImmediatelyButton.IsChecked == true)
                {
                    actionType = SmsFilterActionType.AcceptImmediately;
                }
                else if (DropButton.IsChecked == true)
                {
                    actionType = SmsFilterActionType.Drop;
                }
                else if (PeekButton.IsChecked == true)
                {
                    actionType = SmsFilterActionType.Peek;
                }
                else if (AcceptButton.IsChecked == true)
                {
                    actionType = SmsFilterActionType.Accept;
                }

                // Created set of filters for this application
                SmsFilterRules filterRules = new SmsFilterRules(actionType);
                IList<SmsFilterRule> rules = filterRules.Rules;
                rules.Add(filter);

                // Create a new background task builder.
                BackgroundTaskBuilder taskBuilder = new BackgroundTaskBuilder();

                // Create a new SmsMessageReceivedTrigger.
                SmsMessageReceivedTrigger trigger = new SmsMessageReceivedTrigger(filterRules);

                // Associate the SmsReceived trigger with the background task builder.
                taskBuilder.SetTrigger(trigger);

                // Specify the background task to run when the trigger fires.
                taskBuilder.TaskEntryPoint = BackgroundTaskEntryPoint;

                // Name the background task.
                taskBuilder.Name = BackgroundTaskName;

                // Register the background task.
                BackgroundTaskRegistration taskRegistration = taskBuilder.Register();

                // Associate completed event handler with the new background task.
                taskRegistration.Completed += new BackgroundTaskCompletedEventHandler(OnCompleted);

                UpdateBackgroundTaskUIState(true);
                rootPage.NotifyUser("Registered SMS background task", NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        // Handle request to unregister the background task
        private void Unregister_Click(object sender, RoutedEventArgs e)
        {
            // Loop through all background tasks and unregister our background task            
            foreach (var cur in BackgroundTaskRegistration.AllTasks)
            {
                if (cur.Value.Name == BackgroundTaskName)
                {
                    cur.Value.Unregister(true);
                }
            }

            UpdateBackgroundTaskUIState(false);
            rootPage.NotifyUser("Unregistered SMS background task.", NotifyType.StatusMessage);

        }

        // Handle background task completion event.
        private async void OnCompleted(IBackgroundTaskRegistration sender, BackgroundTaskCompletedEventArgs e)
        {
            // Update the UI with the complrtion status reported by the background task.
            // Dispatch an anonymous task to the UI thread to do the update.
            await sampleDispatcher.RunAsync(CoreDispatcherPriority.Normal,
                () =>
                {
                    try
                    {
                        if ((sender != null) && (e != null))
                        {
                            // this method throws if the event is reporting an error
                            e.CheckResult();

                            // Update the UI with the background task's completion status.
                            // The task stores status in the application data settings indexed by the task ID.
                            var key = sender.TaskId.ToString();
                            var settings = ApplicationData.Current.LocalSettings;
                            BackgroundTaskStatus.Text = settings.Values[key].ToString();
                        }
                    }
                    catch (Exception ex)
                    {
                        rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
                    }
                });
        }


    }
}
