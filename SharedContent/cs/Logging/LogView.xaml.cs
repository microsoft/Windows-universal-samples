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
using System.Linq;
using Windows.ApplicationModel.DataTransfer;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace SDKTemplate.Logging
{
    /// <summary>
    /// Logging levels supported by the LogView control.
    /// The "Always" level is supported only by the Log method.
    /// </summary>
    public enum LogViewLoggingLevel
    {
        Verbose = 0,
        Information,
        Warning,
        Error,
        Critical,
        Always,
    }

    /// 
    /// <summary>
    /// This control can be used to log events from a scenario page.
    /// 
    /// In the XAML markup, you can set the initial logging level by setting
    /// LoggingLevel="Verbose", or any other enum value of LogViewLoggingLevel.
    /// </summary>
    public sealed partial class LogView : UserControl
    {
        public LogView()
        {
            this.InitializeComponent();

            LoggingBrushes = new SolidColorBrush[] {
                (SolidColorBrush)Resources["VerboseLogBrush"],
                (SolidColorBrush)Resources["InformationLogBrush"],
                (SolidColorBrush)Resources["WarningLogBrush"],
                (SolidColorBrush)Resources["ErrorLogBrush"],
                (SolidColorBrush)Resources["CriticalLogBrush"],
                (SolidColorBrush)Resources["AlwaysLogBrush"],
            };

            LogLevelComboBox.SelectedIndex = (int)LoggingLevel;
        }

        public static readonly DependencyProperty LogLevelProperty =
            DependencyProperty.Register("LoggingLevel", typeof(LogViewLoggingLevel), typeof(LogView),
                new PropertyMetadata(LogViewLoggingLevel.Warning, OnLoggingLevelChanged));

        public LogViewLoggingLevel LoggingLevel
        {
            get { return (LogViewLoggingLevel)GetValue(LogLevelProperty); }
            set { SetValue(LogLevelProperty, value); }
        }

        private static void OnLoggingLevelChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var logger = (LogView)d;
            var loggingLevel = (LogViewLoggingLevel)e.NewValue;
            logger.LogLevelComboBox.SelectedIndex = (int)loggingLevel;
        }

        bool IsLogViewFull = false;
        int MaxItemsInLogView = 500;

        SolidColorBrush[] LoggingBrushes;

        /// <summary>
        /// Changes the logging level at which messages are filtered.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void LogLevel_Changed(object sender, SelectionChangedEventArgs e)
        {
            LoggingLevel = (LogViewLoggingLevel)LogLevelComboBox.SelectedIndex;
        }

        /// <summary>
        /// Provides logging with filtering based on UI settings.
        /// Logging functions tangential to main scenarios should specify a log view level less than Always.
        /// </summary>
        /// <param name="message">message to be logged</param>
        /// <param name="level">message category</param>
        public async void Log(string message, LogViewLoggingLevel level = LogViewLoggingLevel.Always)
        {
            if (Dispatcher.HasThreadAccess)
            {
                LogFromUIThread(message, level);
            }
            else
            {
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    LogFromUIThread(message, level);
                });
            }
        }

        private void LogFromUIThread(string message, LogViewLoggingLevel level)
        {
            string logEntry = $"{DateTime.Now:HH:mm:ss.fff} - {message}";

            if (LoggingLevel <= level)
            {
                SolidColorBrush foregroundBrush = LoggingBrushes[(int)level];

                if (IsLogViewFull)
                {
                    // Once the view is full, messages go only to the debug console.
                    System.Diagnostics.Debug.WriteLine(logEntry);
                    return;
                }

                if (LoggingListBox.Items.Count >= MaxItemsInLogView)
                {
                    IsLogViewFull = true;

                    System.Diagnostics.Debug.WriteLine(logEntry);
                    logEntry = "Log window is full";
                    foregroundBrush = new SolidColorBrush(Colors.Red);
                }

                LoggingListBox.Items.Insert(0, new TextBlock() { Text = logEntry, Foreground = foregroundBrush });
            }
            else if (level != LogViewLoggingLevel.Verbose)
            {
                // Non-Verbose messages blocked from the LogView are shown on the debug console.
                System.Diagnostics.Debug.WriteLine($"{level}: {logEntry}");
            }
        }

        private void CopyLogToClipboard_Click(object sender, RoutedEventArgs e)
        {
            var content = new DataPackage();
            content.SetText(String.Join("\r\n", LoggingListBox.Items.Select(item => (item as TextBlock).Text)));
            Clipboard.SetContent(content);
        }

        private void ClearLog_Click(object sender, RoutedEventArgs e)
        {
            ClearLog();
        }

        public async void ClearLog()
        {
            if (Dispatcher.HasThreadAccess)
            {
                LoggingListBox.Items.Clear();
                IsLogViewFull = false;
            }
            else
            {
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, ClearLog);
            }
        }
    }
}
