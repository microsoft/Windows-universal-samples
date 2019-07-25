//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

using System;
using System.Threading.Tasks;
using Windows.Foundation.Diagnostics;
using Windows.Storage;

namespace SDKTemplate
{
    /// <summary>
    /// FileLoggingSessionScenario is a central singleton class which contains the logging-specific 
    /// sample code. 
    /// </summary>
    internal class FileLoggingSessionScenario
        : IDisposable
    {
        #region Primary scenario code

        private string GetTimeStamp()
        {
            DateTime now = DateTime.Now;
            return string.Format(System.Globalization.CultureInfo.InvariantCulture,
                                 "{0:D2}{1:D2}{2:D2}-{3:D2}{4:D2}{5:D2}{6:D3}",
                                 now.Year - 2000,
                                 now.Month,
                                 now.Day,
                                 now.Hour,
                                 now.Minute,
                                 now.Second,
                                 now.Millisecond);
        }

        /// <summary>
        /// Toggle the enabled/disabled status of logging. 
        /// </summary>
        /// <returns>True if the resulting new status is enabled, else false for disabled.</returns>
        public async Task<bool> ToggleLoggingEnabledDisabledAsync()
        {
            CheckDisposed();

            IsBusy = true;

            try
            {
                bool enabled;
                if (session != null)
                {
                    string finalLogFilePath = await CloseSessionSaveFinalLogFile();
                    session.Dispose();
                    session = null;
                    if (StatusChanged != null)
                    {
                        StatusChanged.Invoke(this, new LoggingScenarioEventArgs(LoggingScenarioEventType.LogFileGeneratedAtDisable, finalLogFilePath));
                    }
                    ApplicationData.Current.LocalSettings.Values[LOGGING_ENABLED_SETTING_KEY_NAME] = false;
                    enabled = false;
                }
                else
                {
                    StartLogging();
                    ApplicationData.Current.LocalSettings.Values[LOGGING_ENABLED_SETTING_KEY_NAME] = true;
                    enabled = true;
                }

                if (StatusChanged != null)
                {
                    StatusChanged.Invoke(this, new LoggingScenarioEventArgs(enabled));
                }

                return enabled;
            }
            finally
            {
                IsBusy = false;
            }
        }

        /// <summary>
        /// The logging scenario. Log messages in a loop until 3 log files are created.
        /// </summary>
        /// <returns>The task.</returns>
        public async Task DoScenarioAsync()
        {
            CheckDisposed();

            IsBusy = true;

            try
            {
                await Task.Run(async () =>
                {
                    const int NUMBER_OF_LOG_FILES_TO_GENERATE = 3;
                    int messageIndex = 0;
                    int startFileCount = LogFileGeneratedCount;

                    //
                    // Log large messages until the current log file hits the maximum size.
                    // When the current log file reaches its maximum size, LogFileGeneratedHandler 
                    // will be called.
                    //

                    while (LogFileGeneratedCount - startFileCount < NUMBER_OF_LOG_FILES_TO_GENERATE)
                    {
                        // Since the channel is added to the session at level Warning,
                        // the following is logged because it is logged at level LoggingLevel.Critical.
                        channel.LogMessage(
                            string.Format("Message={0}: Lorem ipsum dolor sit amet, consectetur adipiscing elit. In ligula nisi, vehicula nec eleifend vel, rutrum non dolor. Vestibulum ante ipsum " +
                                          "primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elementum scelerisque accumsan. In hac habitasse platea dictumst.",
                                          ++messageIndex),
                            LoggingLevel.Critical);

                        // Since the channel is added to the session at level Warning,
                        // the following is *not* logged because it is logged at LoggingLevel.Information.
                        channel.LogMessage(
                            string.Format("Message={0}: Lorem ipsum dolor sit amet, consectetur adipiscing elit. In ligula nisi, vehicula nec eleifend vel, rutrum non dolor. Vestibulum ante ipsum " +
                                          "primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elementum scelerisque accumsan. In hac habitasse platea dictumst.",
                                          ++messageIndex),
                            LoggingLevel.Information);

                        int value = 1000000; // one million, 7 digits, 4-bytes as an int, 14 bytes as a wide character string.
                        channel.LogMessage("Value #" + messageIndex.ToString() + "  " + value.ToString(), LoggingLevel.Critical); // value is logged as 14 byte wide character string.
                        channel.LogValuePair("Value #" + messageIndex.ToString(), value, LoggingLevel.Critical); // value is logged as a 4-byte integer.

                        //
                        // Pause every once in a while to simulate application 
                        // activity outside of logging.
                        //

                        if (messageIndex % 50 == 0)
                        {
                            await Task.Delay(10);
                        }
                    }
                });
            }
            finally
            {
                IsBusy = false;
            }
        }

        public void StartLogging()
        {
            CheckDisposed();

            // If no session exists, create one.
            // NOTE: There are use cases where an application
            // may want to create only a channel for sessions outside
            // of the application itself. See MSDN for details. This
            // sample is the common scenario of an app logging events
            // which it wants to place in its own log file, so it creates
            // a session and channel as a pair. The channel is created 
            // during construction of this LoggingScenario class so 
            // it already exsits by the time this function is called. 
            if (session == null)
            {
                session = new FileLoggingSession(DEFAULT_SESSION_NAME);
                session.LogFileGenerated += LogFileGeneratedHandler;
            }

            // This sample adds the channel at level "warning" to 
            // demonstrated how messages logged at more verbose levels
            // are ignored by the session. 
            session.AddLoggingChannel(channel, LoggingLevel.Warning);
        }

        /// <summary>
        // When the log file gets large, the system closes it and starts a new one.
        // The LogFileGenerated event is raised to give the app a chance to move
        // the recently-generated log file to a safe place. When the handler returns,
        // the FileLoggingSession may reuse the file name for a new log file.
        // This function moves the log file to an app-defined location.
        /// </summary>
        /// <param name="sender">The FileLoggingSession which has generated a new file.</param>
        /// <param name="args">Contains a StorageFile field LogFileGeneratedEventArgs.File representing the new log file.</param>
        private async void LogFileGeneratedHandler(IFileLoggingSession sender, LogFileGeneratedEventArgs args)
        {
            LogFileGeneratedCount++;
            StorageFolder sampleAppDefinedLogFolder =
                await ApplicationData.Current.LocalFolder.CreateFolderAsync(OUR_SAMPLE_APP_LOG_FILE_FOLDER_NAME,
                                                                            CreationCollisionOption.OpenIfExists);
            string newLogFileName = "Log-" + GetTimeStamp() + ".etl";
            await args.File.MoveAsync(sampleAppDefinedLogFolder, newLogFileName);
            if (IsPreparingForSuspend == false)
            {
                if (StatusChanged != null)
                {
                    string newLogFileFullPathName = System.IO.Path.Combine(sampleAppDefinedLogFolder.Path, newLogFileName);
                    StatusChanged.Invoke(this, new LoggingScenarioEventArgs(LoggingScenarioEventType.LogFileGenerated, newLogFileFullPathName));
                }
            }
        }

        private async Task<string> CloseSessionSaveFinalLogFile()
        {
            // Save the final log file before closing the session.
            StorageFile finalFileBeforeSuspend = await session.CloseAndSaveToFileAsync();
            if (finalFileBeforeSuspend != null)
            {
                LogFileGeneratedCount++;
                // Get the the app-defined log file folder. 
                StorageFolder sampleAppDefinedLogFolder =
                    await ApplicationData.Current.LocalFolder.CreateFolderAsync(OUR_SAMPLE_APP_LOG_FILE_FOLDER_NAME,
                                                                                CreationCollisionOption.OpenIfExists);
                // Create a new log file name based on a date/time stamp.
                string newLogFileName = "Log-" + GetTimeStamp() + ".etl";
                // Move the final log into the app-defined log file folder. 
                await finalFileBeforeSuspend.MoveAsync(sampleAppDefinedLogFolder, newLogFileName);
                // Return the path to the log folder.
                return System.IO.Path.Combine(sampleAppDefinedLogFolder.Path, newLogFileName);
            }
            else
            {
                return null;
            }
        }

        #endregion

        #region Scenario code for tracking a LoggingChannel's enablement status and related logging level.

        /// <summary>
        /// This boolean tracks whether or not there are any
        /// sessions listening to the app's channel. This is
        /// adjusted as the channel's LoggingEnabled event is 
        /// raised. Search for OnChannelLoggingEnabled for 
        /// more information.
        /// </summary>
        private bool isChannelEnabled = false;

        /// <summary>
        /// This is the current maximum level of listeners of
        /// the application's channel. It is adjusted as the 
        /// channel's LoggingEnabled event is raised. Search
        /// for OnChannelLoggingEnabled for more information.
        /// </summary>
        private LoggingLevel channelLoggingLevel = LoggingLevel.Verbose;

        void OnChannelLoggingEnabled(ILoggingChannel sender, object args)
        {
            // This method is called when the channel is informing us of channel-related state changes.
            // Save new channel state. These values can be used for advanced logging scenarios where, 
            // for example, it's desired to skip blocks of logging code if the channel is not being
            // consumed by any sessions. 
            isChannelEnabled = sender.Enabled;
            channelLoggingLevel = sender.Level;
        }

        #endregion

        #region Scenario code for suspend/resume.

        private const string LOGGING_ENABLED_SETTING_KEY_NAME = Prefix + "LoggingEnabled";
        private const string LOGFILEGEN_BEFORE_SUSPEND_SETTING_KEY_NAME = Prefix + "LogFileGeneratedBeforeSuspend";

        public bool IsPreparingForSuspend { get; private set; }

        public bool IsLoggingEnabled
        {
            get
            {
                return session != null;
            }
        }

        /// <summary>
        /// Prepare this scenario for suspend. 
        /// </summary>
        /// <returns></returns>
        public async Task PrepareToSuspendAsync()
        {
            CheckDisposed();

            if (session != null)
            {
                IsPreparingForSuspend = true;

                try
                {
                    // Before suspend, save any final log file.
                    string finalFileBeforeSuspend = await CloseSessionSaveFinalLogFile();
                    session.Dispose();
                    session = null;
                    // Save values used when the app is resumed or started later.
                    // Logging is enabled.
                    ApplicationData.Current.LocalSettings.Values[LOGGING_ENABLED_SETTING_KEY_NAME] = true;
                    // Save the log file name saved at suspend so the sample UI can be 
                    // updated on resume with that information. 
                    ApplicationData.Current.LocalSettings.Values[LOGFILEGEN_BEFORE_SUSPEND_SETTING_KEY_NAME] = finalFileBeforeSuspend;
                }
                finally
                {
                    IsPreparingForSuspend = false;
                }
            }
            else
            {
                // Save values used when the app is resumed or started later.
                // Logging is not enabled and no log file was saved.
                ApplicationData.Current.LocalSettings.Values[LOGGING_ENABLED_SETTING_KEY_NAME] = false;
                ApplicationData.Current.LocalSettings.Values[LOGFILEGEN_BEFORE_SUSPEND_SETTING_KEY_NAME] = null;
            }
        }

        /// <summary>
        /// This is called when the app is either resuming or starting. 
        /// It will enable logging if the app has never been started before
        /// or if logging had been enabled the last time the app was running.
        /// </summary>
        public void ResumeLoggingIfApplicable()
        {
            CheckDisposed();

            object loggingEnabled;
            if (ApplicationData.Current.LocalSettings.Values.TryGetValue(LOGGING_ENABLED_SETTING_KEY_NAME, out loggingEnabled) == false)
            {
                ApplicationData.Current.LocalSettings.Values[LOGGING_ENABLED_SETTING_KEY_NAME] = true;
                loggingEnabled = ApplicationData.Current.LocalSettings.Values[LOGGING_ENABLED_SETTING_KEY_NAME];
            }

            if (loggingEnabled is bool && (bool)loggingEnabled == true)
            {
                StartLogging();
            }

            // When the sample suspends, it retains state as to whether or not it had
            // generated a new log file at the last suspension. This allows any
            // UI to be updated on resume to reflect that fact. 
            object LogFileGeneratedBeforeSuspendObject;
            if (ApplicationData.Current.LocalSettings.Values.TryGetValue(LOGFILEGEN_BEFORE_SUSPEND_SETTING_KEY_NAME, out LogFileGeneratedBeforeSuspendObject) &&
                LogFileGeneratedBeforeSuspendObject != null &&
                LogFileGeneratedBeforeSuspendObject is string)
            {
                if (StatusChanged != null)
                {
                    StatusChanged.Invoke(this,
                                         new LoggingScenarioEventArgs(LoggingScenarioEventType.LogFileGeneratedAtSuspend,
                                                                    (string)LogFileGeneratedBeforeSuspendObject));

                }
                ApplicationData.Current.LocalSettings.Values[LOGFILEGEN_BEFORE_SUSPEND_SETTING_KEY_NAME] = null;
            }
        }

        #endregion

        #region Helper functions/properties/events to support sample UI feedback.

        public event EventHandler<LoggingScenarioEventArgs> StatusChanged;

        private bool isBusy;
        /// <summary>
        /// True if the scenario is busy, or false if not busy.
        /// The UI can use this to affect UI controls.
        /// </summary>
        public bool IsBusy
        {
            get
            {
                return isBusy;
            }

            private set
            {
                isBusy = value;
                if (StatusChanged != null)
                {
                    StatusChanged.Invoke(this, new LoggingScenarioEventArgs(LoggingScenarioEventType.BusyStatusChanged));
                }
            }
        }

        /// <summary>
        /// The number of times LogFileGeneratedHandler has been called.
        /// </summary>

        public int LogFileGeneratedCount { get; private set; }

        #endregion

        #region LoggingScenario constants and privates.

        private const string Prefix = "FileLoggingSessionScenario_";
        public const string DEFAULT_SESSION_NAME = Prefix + "Session";
        public const string DEFAULT_CHANNEL_NAME = Prefix + "Channel";

        /// <summary>
        /// LoggingScenario moves generated logs files into the 
        /// this folder under the LocalState folder.
        /// </summary>
        public const string OUR_SAMPLE_APP_LOG_FILE_FOLDER_NAME = Prefix + "LogFiles";

        /// <summary>
        /// <summary>
        /// The sample's one session.
        /// </summary>
        private FileLoggingSession session;

        /// <summary>
        /// The sample's one channel.
        /// </summary>
        private LoggingChannel channel;

        #endregion

        #region LoggingScenario constructor and singleton accessor.

        /// <summary>
        /// Disallow creation of instances beyond the one instance for the process.
        /// The one instance is accessible via the Instance property (see below).
        /// </summary>
        private FileLoggingSessionScenario()
        {
            LogFileGeneratedCount = 0;

            // This sample demonstrates LoggingSession using the Windows 8.1 LoggingChannel APIs.
#pragma warning disable 618 // LoggingChannel is obsolete
            channel = new LoggingChannel(DEFAULT_CHANNEL_NAME);
#pragma warning restore

            channel.LoggingEnabled += OnChannelLoggingEnabled;

            App.Current.Suspending += OnAppSuspending;
            App.Current.Resuming += OnAppResuming;

            // If the app is being launched (not resumed), the 
            // following call will activate logging if it had been
            // activated during the last suspend. 
            ResumeLoggingIfApplicable();
        }

        ~FileLoggingSessionScenario()
        {
            Dispose(false);
        }

        async void OnAppSuspending(object sender, Windows.ApplicationModel.SuspendingEventArgs e)
        {
            // Get a deferral before performing any async operations
            // to avoid suspension prior to LoggingScenario completing 
            // PrepareToSuspendAsync().
            var deferral = e.SuspendingOperation.GetDeferral();

            // Prepare logging for suspension.
            await PrepareToSuspendAsync();

            // From LoggingScenario's perspective, it's now okay to 
            // suspend, so release the deferral. 
            deferral.Complete();
        }

        void OnAppResuming(object sender, object e)
        {
            // If logging was active at the last suspend,
            // ResumeLoggingIfApplicable will re-activate 
            // logging.
            ResumeLoggingIfApplicable();
        }

        /// The app's one and only LoggingScenario instance.
        /// </summary>
        static private FileLoggingSessionScenario loggingScenario;

        /// <summary>
        /// A method to allowing callers to access the app's one and only LoggingScenario instance.
        /// </summary>
        /// <returns>The logging helper.</returns>
        static public FileLoggingSessionScenario Instance
        {
            get
            {
                if (loggingScenario == null)
                {
                    loggingScenario = new FileLoggingSessionScenario();
                }
                return loggingScenario;
            }
        }

        #endregion

        #region IDisposable handling

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (isDisposed == false)
            {
                isDisposed = true;

                if (disposing)
                {
                    if (channel != null)
                    {
                        channel.Dispose();
                        channel = null;
                    }

                    if (session != null)
                    {
                        session.Dispose();
                        session = null;
                    }
                }
            }
        }

        /// <summary>
        /// Set to 'true' if Dispose() has been called.
        /// </summary>
        private bool isDisposed = false;

        /// <summary>
        /// Helper function for other methods to call to check Dispose() state.
        /// </summary>
        private void CheckDisposed()
        {
            if (isDisposed)
            {
                throw new ObjectDisposedException("LoggingScenario");
            }
        }

        #endregion
    }
}
