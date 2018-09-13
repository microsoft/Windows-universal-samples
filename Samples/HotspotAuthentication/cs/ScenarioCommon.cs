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

using HotspotAuthenticationTask;
using System;
using Windows.ApplicationModel.Background;
using Windows.UI.Core;

namespace SDKTemplate
{
    // A delegate type for hooking up foreground authentication notifications.
    public class BackgroundAuthenticationCompletedEventArgs
    {
        public BackgroundAuthenticationCompletedEventArgs(Exception exception)
        {
            ExtendedError = exception;
        }
        public bool Succeeded => ExtendedError == null;
        public Exception ExtendedError { get; private set; }
    }

    public delegate void BackgroundAuthenticationCompletedEventHandler(ScenarioCommon sender, BackgroundAuthenticationCompletedEventArgs e);

    // Shared code for all scenario pages
    public class ScenarioCommon
    {
        public static ScenarioCommon Instance { get; } = new ScenarioCommon();

        // The entry point name of the background task handler:

        public const string BackgroundTaskEntryPoint = "HotspotAuthenticationTask.AuthenticationTask";

        // The (arbitrarily chosen) name assigned to the background task:
        public const string BackgroundTaskName = "AuthenticationBackgroundTask";

        // A delegate for subscribing for foreground authentication events
        public BackgroundAuthenticationCompletedEventHandler BackgroundAuthenticationCompleted;

        // A pointer back to the main page.  This is needed to call methods in MainPage such as NotifyUser()
        MainPage rootPage = MainPage.Current;

        // A flag to remember if a background task handler has been registered
        private bool HasRegisteredBackgroundTaskHandler = false;

        /// <summary>
        /// Register completion handler for registered background task on application startup.
        /// </summary>
        /// <returns>True if a registerd task was found</returns>
        public bool RegisteredCompletionHandlerForBackgroundTask()
        {
            if (!HasRegisteredBackgroundTaskHandler)
            {
                try
                {
                    // Associate background task completed event handler with background task.
                    foreach (var cur in BackgroundTaskRegistration.AllTasks)
                    {
                        if (cur.Value.Name == BackgroundTaskName)
                        {
                            cur.Value.Completed += new BackgroundTaskCompletedEventHandler(OnBackgroundTaskCompleted);
                            HasRegisteredBackgroundTaskHandler = true;
                            break;
                        }
                    }
                }
                catch (Exception ex)
                {
                    rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
                }
            }
            return HasRegisteredBackgroundTaskHandler;
        }

        /// <summary>
        /// Background task completion handler. When authenticating through the foreground app, this triggers the authentication flow if the app is currently running.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void OnBackgroundTaskCompleted(IBackgroundTaskRegistration sender, BackgroundTaskCompletedEventArgs e)
        {
            if (sender.Name == BackgroundTaskName)
            {
                Exception exception = null;
                try
                {
                    // If the background task threw an exception, re-raise it here so we can pass it to the event handler.
                    e.CheckResult();
                }
                catch (Exception ex)
                {
                    exception = ex;
                }

                if (!ConfigStore.AuthenticateThroughBackgroundTask)
                {
                    BackgroundAuthenticationCompleted?.Invoke(this, new BackgroundAuthenticationCompletedEventArgs(null));
                }
            }
        }
    }
}
