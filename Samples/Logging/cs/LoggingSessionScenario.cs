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
    /// LoggingSessionScenario is a central singleton class which contains the logging-specific
    /// sample code.
    /// </summary>
    internal class LoggingSessionScenario
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
        public bool ToggleLoggingEnabledDisabled()
        {
            CheckDisposed();

            IsBusy = true;

            try
            {
                bool enabled;
                if (session != null)
                {
                    session.Dispose();
                    session = null;
                    ApplicationData.Current.LocalSettings.Values["LoggingEnabled"] = false;
                    enabled = false;
                }
                else
                {
                    StartLogging();
                    ApplicationData.Current.LocalSettings.Values["LoggingEnabled"] = true;
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
                    int lastDelay = 0;
                    int lastAppError = 0;
                    int startFileCount = LogFileGeneratedCount;

                    //
                    // Log large messages until the current log file hits the maximum size.
                    // When the current log file reaches its maximum size, LogFileGeneratedHandler
                    // will be called.
                    //

                    while (LogFileGeneratedCount - startFileCount < NUMBER_OF_LOG_FILES_TO_GENERATE)
                    {
                        try
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
                            channel.LogMessage("Value #" + (++messageIndex).ToString() + "  " + value.ToString(), LoggingLevel.Critical); // value is logged as 14 byte wide character string.
                            channel.LogValuePair("Value #" + (++messageIndex).ToString(), value, LoggingLevel.Critical); // value is logged as a 4-byte integer.

                            //
                            // Pause every once in a while to simulate application
                            // activity outside of logging.
                            //

                            if (messageIndex - lastDelay > 100)
                            {
                                lastDelay = messageIndex;
                                await Task.Delay(10);
                            }

                            //
                            // Every once in a while, simulate an application error
                            // which causes the app to save the current snapshot
                            // of logging events in memory to a disk ETL file.
                            //

                            if (messageIndex - lastAppError >= 25000)
                            {
                                lastAppError = messageIndex;

                                // Before simulating an application error, demonstrate LoggingActivity.
                                // A LoggingActivity outputs a pair of begin and end messages to the channel.
                                using (new LoggingActivity("Add two numbers.", channel, LoggingLevel.Critical))
                                {
                                    int oneNumber = 100;
                                    int anotherNumber = 200;
                                    int total = oneNumber + anotherNumber;
                                    channel.LogMessage(string.Format("Message={0}: The result of adding two numbers: {1}", ++messageIndex, total), LoggingLevel.Critical);
                                } // The LoggingActivity instance will log the second message at this scope exit.

                                // Simulate an application error.
                                throw new AppException("Some bad app error occurred.");
                            }
                        }
                        catch (AppException e)
                        {
                            // Log the exception string.
                            channel.LogMessage("Exception occurrred: " + e.ToString(), LoggingLevel.Error);

                            // Save the memory log buffer to file.
                            Task<string> op = SaveLogInMemoryToFileAsync();
                            op.Wait();
                            if (op.IsFaulted)
                            {
                                throw new Exception("After an app error occurred, there was a failure to save the log file.", op.Exception);
                            }
                            LogFileGeneratedCount++;
                            // For the sample, update the UI to show a log file has been generated.
                            if (StatusChanged != null)
                            {
                                StatusChanged.Invoke(this, new LoggingScenarioEventArgs(LoggingScenarioEventType.LogFileGenerated, op.Result));
                            }
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
            // during construction of this LoggingSessionScenario class so
            // it already exsits by the time this function is called.
            if (session == null)
            {
                session = new LoggingSession(DEFAULT_SESSION_NAME);
            }

            // This sample adds the channel at level "warning" to
            // demonstrated how messages logged at more verbose levels
            // are ignored by the session.
            session.AddLoggingChannel(channel, LoggingLevel.Warning);
        }

        private async Task<string> SaveLogInMemoryToFileAsync()
        {
            StorageFolder sampleAppDefinedLogFolder =
                await ApplicationData.Current.LocalFolder.CreateFolderAsync(OUR_SAMPLE_APP_LOG_FILE_FOLDER_NAME,
                                                                            CreationCollisionOption.OpenIfExists);
            string newLogFileName = "Log-" + GetTimeStamp() + ".etl";
            StorageFile newLogFile = await session.SaveToFileAsync(sampleAppDefinedLogFolder, newLogFileName);
            if (newLogFile != null)
            {
                return newLogFile.Path;
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

        private const string LoggingEnabledKey = Prefix + "LoggingEnabled";

        public bool IsLoggingEnabled
        {
            get
            {
                return session != null;
            }
        }

        public void PrepareToSuspend()
        {
            CheckDisposed();

            ApplicationData.Current.LocalSettings.Values["LoggingEnabled"] = session != null;
        }

        public void ResumeLoggingIfApplicable()
        {
            CheckDisposed();

            object loggingEnabled;
            if (ApplicationData.Current.LocalSettings.Values.TryGetValue("LoggingEnabled", out loggingEnabled) == false)
            {
                ApplicationData.Current.LocalSettings.Values["LoggingEnabled"] = true;
                loggingEnabled = ApplicationData.Current.LocalSettings.Values["LoggingEnabled"];
            }

            if (loggingEnabled is bool && (bool)loggingEnabled == true)
            {
                StartLogging();
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

        #region LoggingSessionScenario constants and privates.

        private const string Prefix = "LoggingSessionScenario_";
        public const string DEFAULT_SESSION_NAME = Prefix + "Session";
        public const string DEFAULT_CHANNEL_NAME = Prefix + "Channel";

        /// <summary>
        /// LoggingSessionScenario moves generated logs files into the
        /// this folder under the LocalState folder.
        /// </summary>
        public const string OUR_SAMPLE_APP_LOG_FILE_FOLDER_NAME = Prefix + "LogFiles";

        /// <summary>
        /// <summary>
        /// The sample's one session.
        /// </summary>
        private LoggingSession session;

        /// <summary>
        /// The sample's one channel.
        /// </summary>
        private LoggingChannel channel;

        #endregion

        #region LoggingSessionScenario constructor and singleton accessor.

        /// <summary>
        /// Disallow creation of instances beyond the one instance for the process.
        /// The one instance is accessible via the Instance property (see below).
        /// </summary>
        private LoggingSessionScenario()
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

        ~LoggingSessionScenario()
        {
            Dispose(false);
        }

        void OnAppSuspending(object sender, Windows.ApplicationModel.SuspendingEventArgs e)
        {
            // Get a deferral before performing any async operations
            // to avoid suspension prior to LoggingSessionScenario completing
            // PrepareToSuspendAsync().
            var deferral = e.SuspendingOperation.GetDeferral();
            // Prepare logging for suspension.
            PrepareToSuspend();
            // From LoggingSessionScenario's perspective, it's now okay to
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

        /// The app's one and only LoggingSessionScenario instance.
        /// </summary>
        static private LoggingSessionScenario loggingSessionScenario;

        /// <summary>
        /// A method to allowing callers to access the app's one and only LoggingSessionScenario instance.
        /// </summary>
        /// <returns>The logging helper.</returns>
        static public LoggingSessionScenario Instance
        {
            get
            {
                if (loggingSessionScenario == null)
                {
                    loggingSessionScenario = new LoggingSessionScenario();
                }
                return loggingSessionScenario;
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
                throw new ObjectDisposedException("LoggingSessionScenario");
            }
        }

        #endregion
    }

    #region Exception to simulate an app error.

    class AppException : Exception
    {
        public AppException()
        {
        }

        public AppException(string message)
            : base(message)
        {
        }
    }

    #endregion
}
