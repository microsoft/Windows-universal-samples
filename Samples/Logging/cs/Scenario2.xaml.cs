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
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// UI for the LoggingSession sample.
    /// </summary>
    public sealed partial class Scenario2
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        internal LoggingSessionScenario LoggingSessionScenario { get { return LoggingSessionScenario.Instance; } }

        public Scenario2()
        {
            // This sample UI is interested in events from
            // the LoggingSessionScenario class so the UI can be updated.
            LoggingSessionScenario.StatusChanged += LoggingSessionScenario_StatusChanged;
            this.InitializeComponent();
        }

        async void LoggingSessionScenario_StatusChanged(object sender, LoggingScenarioEventArgs e)
        {
            if (e.Type == LoggingScenarioEventType.BusyStatusChanged)
            {
                UpdateControls();
            }
            else if (e.Type == LoggingScenarioEventType.LogFileGenerated)
            {
                await AddLogFileMessageDispatch("LogFileGenerated", e.LogFilePath);
            }
            else if (e.Type == LoggingScenarioEventType.LoggingEnabledDisabled)
            {
                await AddMessageDispatch(string.Format("Logging has been {0}.", e.Enabled ? "enabled" : "disabled"));
            }
        }

        ScrollViewer FindScrollViewer(DependencyObject depObject)
        {
            if (depObject == null)
            {
                return null;
            }

            int countThisLevel = Windows.UI.Xaml.Media.VisualTreeHelper.GetChildrenCount(depObject);
            if (countThisLevel <= 0)
            {
                return null;
            }

            for (int childIndex = 0; childIndex < countThisLevel; childIndex++)
            {
                DependencyObject childDepObject = Windows.UI.Xaml.Media.VisualTreeHelper.GetChild(depObject, childIndex);
                if (childDepObject is ScrollViewer)
                {
                    return (ScrollViewer)childDepObject;
                }

                ScrollViewer svFromChild = FindScrollViewer(childDepObject);
                if (svFromChild != null)
                {
                    return svFromChild;
                }
            }

            return null;
        }

        /// <summary>
        /// Add a message to the UI control which displays status while the sample is running.
        /// </summary>
        /// <param name="message">The message to append to the UI log.</param>
        public void AddMessage(string message)
        {
            StatusMessageList.Text += message + "\r\n";
            StatusMessageList.Select(StatusMessageList.Text.Length, 0);

            ScrollViewer svFind = FindScrollViewer(StatusMessageList);
            if (svFind != null)
            {
                svFind.ChangeView(null, StatusMessageList.ActualHeight, null);
            }
        }

        /// <summary>
        /// Dispatch to the UI thread and add a message to the UI control which displays status while the sample is running.
        /// </summary>
        /// <param name="message">The message to append to the UI log.</param>
        public async Task AddLogFileMessageDispatch(string message, string path)
        {
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.High, () =>
            {
                string finalMessage;
                if (path != null && path.Length > 0)
                {
                    var fileName = System.IO.Path.GetFileName(path);
                    var directoryName = System.IO.Path.GetDirectoryName(path);
                    finalMessage = message + ": " + fileName;
                    ViewLogInfo.Text =
                        $"Log folder: \"{directoryName}\"\r\n" +
                        $"- To view with tracerpt: tracerpt.exe \"{path}\" -of XML -o LogFile.xml\r\n" +
                        $"- To view with Windows Performance Toolkit (WPT):\r\n" +
                        $"   xperf -merge \"{path}\" merged.etl\r\n" +
                        $"   wpa.exe merged.etl";
                }
                else
                {
                    finalMessage = string.Format("{0}: none, nothing logged since saving the last file.", message);
                }
                AddMessage(finalMessage);
            }).AsTask();
        }

        /// <summary>
        /// Dispatch to the UI thread and add a message to the UI log.
        /// </summary>
        /// <param name="message">The message to appened to the UI log.</param>
        /// <returns>The task.</returns>
        public async Task AddMessageDispatch(string message)
        {
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.High, () =>
            {
                AddMessage(message);
            }).AsTask();
        }

        /// <summary>
        /// Adjust UI controls based on what the sample is doing.
        /// </summary>
        private void UpdateControls()
        {
            if (LoggingSessionScenario.Instance.IsLoggingEnabled)
            {
                InputTextBlock1.Text = "Logging is enabled. Click 'Disable Logging' to disable logging. With logging enabled, you can click 'Log Messages' to use the logging API to generate log files.";
                EnableDisableLoggingButton.Content = "Disable Logging";

                if (LoggingSessionScenario.Instance.IsBusy)
                {
                    DoScenarioButton.IsEnabled = false;
                    EnableDisableLoggingButton.IsEnabled = false;
                }
                else
                {
                    DoScenarioButton.IsEnabled = true;
                    EnableDisableLoggingButton.IsEnabled = true;
                }
            }
            else
            {
                InputTextBlock1.Text = "Logging is disabled. Click 'Enable Logging' to enable logging. After you enable logging you can click 'Log Messages' to use the logging API to generate log files.";
                EnableDisableLoggingButton.Content = "Enable Logging";
                DoScenarioButton.IsEnabled = false;
                if (LoggingSessionScenario.Instance.IsBusy)
                {
                    EnableDisableLoggingButton.IsEnabled = false;
                }
                else
                {
                    EnableDisableLoggingButton.IsEnabled = true;
                }
            }
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            UpdateControls();
        }

        /// <summary>
        /// Enabled/disabled logging.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The event arguments.</param>
        private void EnableDisableLogging(object sender, RoutedEventArgs e)
        {
            LoggingSessionScenario loggingSessionScenario = LoggingSessionScenario.Instance;

            if (loggingSessionScenario.IsLoggingEnabled)
            {
                rootPage.NotifyUser("Disabling logging...", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("Enabling logging...", NotifyType.StatusMessage);
            }

            LoggingSessionScenario.Instance.ToggleLoggingEnabledDisabled();

            if (loggingSessionScenario.IsLoggingEnabled)
            {
                rootPage.NotifyUser("Logging enabled.", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("Logging disabled.", NotifyType.StatusMessage);
            }

            UpdateControls();
        }

        /// <summary>
        /// Run a sample scenario which logs lots of messages to produce several log files.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The event args.</param>
        private async void DoScenario(object sender, RoutedEventArgs e)
        {
            DoScenarioButton.IsEnabled = false;
            rootPage.NotifyUser("Scenario running...", NotifyType.StatusMessage);
            await LoggingSessionScenario.DoScenarioAsync();
            rootPage.NotifyUser("Scenario finished.", NotifyType.StatusMessage);
            DoScenarioButton.IsEnabled = true;
        }
    }
}
