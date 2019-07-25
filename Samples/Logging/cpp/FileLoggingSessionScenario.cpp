//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#include "pch.h"
#include "FileLoggingSessionScenario.h"
#include "LoggingScenarioHelpers.h"

#define SCENARIO_PREFIX                     L"FileLoggingSessionScenario_"
#define DEFAULT_SESSION_NAME                SCENARIO_PREFIX L"Session"
#define DEFAULT_CHANNEL_NAME                SCENARIO_PREFIX L"Channel"
#define OUR_SAMPLE_APP_LOG_FILE_FOLDER_NAME SCENARIO_PREFIX L"LogFiles"
#define LOGGING_ENABLED_SETTING_KEY_NAME    SCENARIO_PREFIX L"LoggingEnabled"
#define LOGFILEGEN_BEFORE_SUSPEND_SETTING_KEY_NAME SCENARIO_PREFIX L"LogFileGeneratedBeforeSuspend"

using namespace concurrency;
using namespace Windows::Foundation::Diagnostics;
using namespace Windows::Storage;
using namespace SDKTemplate;

FileLoggingSessionScenario^ FileLoggingSessionScenario::_instance;

FileLoggingSessionScenario::FileLoggingSessionScenario()
{
    _logFileGeneratedCount = 0;
    _isPreparingForSuspend = false;
    _isBusy = false;

    // Create the logging channel.
    // When an app logs messages to a channel, the messges will go 
    // to any sessions which are consuming messages from the channel.
#pragma warning(push)
#pragma warning(disable : 4973)
    _channel = ref new Windows::Foundation::Diagnostics::LoggingChannel(DEFAULT_CHANNEL_NAME);
#pragma warning(pop)

    // This sample listens to the channel's events so it can track 
    // information about the channel's enablement status. 
    _channel->LoggingEnabled += ref new Windows::Foundation::TypedEventHandler<Windows::Foundation::Diagnostics::ILoggingChannel ^, Platform::Object ^>(this, &FileLoggingSessionScenario::OnChannelLoggingEnabled);

    App::Current->Suspending += ref new Windows::UI::Xaml::SuspendingEventHandler(this, &FileLoggingSessionScenario::OnAppSuspending);
    App::Current->Resuming += ref new Windows::Foundation::EventHandler<Platform::Object ^>(this, &FileLoggingSessionScenario::OnAppResuming);

    // If the app is being launched (not resumed), the 
    // following call will activate logging if it had been 
    // active at the time of the last suspend. 
    ResumeLoggingIfApplicable();
}

FileLoggingSessionScenario::~FileLoggingSessionScenario()
{
}

void FileLoggingSessionScenario::OnAppSuspending(Platform::Object ^sender, Windows::ApplicationModel::SuspendingEventArgs ^e)
{
    (void) sender;    // Unused parameter

    // Get a deferral before performing any async operations
    // to avoid suspension prior to LoggingScenario completing 
    // PrepareToSuspendAsync().
    auto deferral = e->SuspendingOperation->GetDeferral();
    PrepareToSuspendAsync().then([=](task<void> previousTask)
    {
        // From LoggingScenario's perspective, it's now okay to 
        // suspend, so release the deferral. 
        deferral->Complete();
    });
}

void FileLoggingSessionScenario::OnAppResuming(Platform::Object ^sender, Platform::Object ^args)
{
    // If logging was active at the last suspend,
    // ResumeLoggingIfApplicable will re-activate 
    // logging.
    ResumeLoggingIfApplicable();
}

void FileLoggingSessionScenario::OnChannelLoggingEnabled(ILoggingChannel^ sender, Platform::Object^ args)
{
    // This method is called when the channel is informing us of channel-related state changes.
    // Save new channel state. These values can be used for advanced logging scenarios where, 
    // for example, it's desired to skip blocks of logging code if the channel is not being
    // consumed by any sessions. 
    _isChannelEnabled = sender->Enabled;
    _channelLoggingLevel = sender->Level;
}

// When the log file gets large, the system closes it and starts a new one.
// The LogFileGenerated event is raised to give the app a chance to move
// the recently-generated log file to a safe place. When the handler returns,
// the FileLoggingSession may reuse the file name for a new log file.
// This function moves the log file to an app-defined location.
void FileLoggingSessionScenario::OnLogFileGenerated(IFileLoggingSession^ sender, LogFileGeneratedEventArgs^ args)
{
    InterlockedIncrement(&_logFileGeneratedCount);

    // ContinuationData allows information to be
    // shared across the below async operations.
    struct ContinuationData
    {
        StorageFolder^ ourSampleAppLogFolder;
        StorageFile^ generatedLogFile;
        Platform::String^ logFileName;
    };
    auto data = std::make_shared<ContinuationData>();
    data->generatedLogFile = args->File;
    // Create the app-defined folder under the app's local folder.
    // An app defines where/how it wants to store log files. 
    // This sample uses OUR_SAMPLE_APP_LOG_FILE_FOLDER_NAME under
    // the app's local folder. 
    create_task(ApplicationData::Current->LocalFolder->CreateFolderAsync(OUR_SAMPLE_APP_LOG_FILE_FOLDER_NAME, CreationCollisionOption::OpenIfExists))
        .then([this, data](StorageFolder^ ourSampleAppLogFolder)
        {
            // After creating/opening our sample app's log folder, 
            // save it for use in a continuation further below. 
            data->ourSampleAppLogFolder = ourSampleAppLogFolder;
            // Create a new log file name, and save it for use 
            // in a continuation further below. 
            data->logFileName = "Log-" + ref new Platform::String(GetTimeStamp().c_str()) + ".etl";
            // Move the log file to our sample app's log folder. 
            return data->generatedLogFile->MoveAsync(ourSampleAppLogFolder, data->logFileName, Windows::Storage::NameCollisionOption::FailIfExists);
        })
        .then([this, data](task<void> previousTask)
        {
            // Update the UI to show the new log file.
            if (_isPreparingForSuspend == false)
            {
                StatusChanged(this, ref new LoggingScenarioEventArgs(LoggingScenarioEventType::LogFileGenerated, std::wstring((data->ourSampleAppLogFolder->Path + "\\" + data->logFileName)->Data())));
            }
        });
}

task<StorageFile^> FileLoggingSessionScenario::CloseSessionSaveFinalLogFile()
{

    // ContinuationData allows information to be
    // shared across the below async operations.
    struct ContinuationData
    {
        StorageFolder^ ourSampleAppLogFolder;
        Platform::String^ logFileName;
    };
    auto data = std::make_shared<ContinuationData>();

    // Tell the session to save its final log file.
    return create_task(_session->CloseAndSaveToFileAsync())
        .then([this, data](StorageFile^ finalFileBeforeSuspend)
        {
            if (finalFileBeforeSuspend != nullptr)
            {
                // A final log file was created. 
                // Move it to the sample-defined log folder. 
                // First, get create/open our sample-defined log folder.
                return create_task(ApplicationData::Current->LocalFolder->CreateFolderAsync(OUR_SAMPLE_APP_LOG_FILE_FOLDER_NAME, CreationCollisionOption::OpenIfExists))
                    .then([this, finalFileBeforeSuspend, data](StorageFolder^ ourSampleAppLogFolder)
                    {
                        // Next, move the log file to the sample-defined log folder. 
                        data->ourSampleAppLogFolder = ourSampleAppLogFolder;
                        data->logFileName = "Log-" + ref new Platform::String(GetTimeStamp().c_str()) + ".etl";
                        return finalFileBeforeSuspend->MoveAsync(ourSampleAppLogFolder, data->logFileName, Windows::Storage::NameCollisionOption::FailIfExists);
                    })
                    .then([this, data](task<void> previousTask)
                    {
                        // Return a StorageFile representing the log file. 
                        return StorageFile::GetFileFromPathAsync(data->ourSampleAppLogFolder->Path + "\\" + data->logFileName);
                    });
            }
            
            // Async operation need to return consistent types.
            // If the session did not produce any log file, 
            // return a task which produces a null StorageFile.
            return task<StorageFile^>([]() -> StorageFile^
            {
                return nullptr;
            });     
        });
}

// Handle a request to toggle logging enable/disable status.
task<bool> FileLoggingSessionScenario::ToggleLoggingEnabledDisabledAsync()
{
    IsBusy = true;
    try
    {
        if (_session != nullptr)
        {
            // Save any final log file.
            return create_task(CloseSessionSaveFinalLogFile()).then([this](StorageFile^ finalLogFilePath)
            {
                // Cleanup the session and let the sample UI know of the new status.
                delete _session;
                _session = nullptr;
                StatusChanged(this, ref new LoggingScenarioEventArgs(LoggingScenarioEventType::LogFileGeneratedAtDisable, finalLogFilePath == nullptr ? L"" : finalLogFilePath->Path->Data()));
                SetAppLocalSettingsValue(LOGGING_ENABLED_SETTING_KEY_NAME, false);
                StatusChanged(this, ref new LoggingScenarioEventArgs(false));
                IsBusy = false;
                return false;
            });
        }
        else
        {
            // Enable logging and let the UI know of the new status.
            StartLogging();
            SetAppLocalSettingsValue(LOGGING_ENABLED_SETTING_KEY_NAME, true);
            StatusChanged(this, ref new LoggingScenarioEventArgs(true));
            IsBusy = false;
            return task<bool>([]() -> bool
            {
                return true;
            });
        }
    }
    catch (...)
    {
        IsBusy = false;
        throw;
    }
}

void FileLoggingSessionScenario::StartLogging()
{
    // If no session exists, create one.
    // NOTE: There are use cases where an application
    // may want to create only a channel for sessions outside
    // of the application itself. See MSDN for details. This
    // sample is the common scenario of an app logging events
    // which it wants to place in its own log file, so it creates
    // a session and channel as a pair. The channel is created 
    // during construction of this LoggingScenario class so 
    // it already exsits by the time this function is called. 
    if (_session == nullptr)
    {
        _session = ref new Windows::Foundation::Diagnostics::FileLoggingSession(DEFAULT_SESSION_NAME);
        _session->LogFileGenerated += ref new Windows::Foundation::TypedEventHandler<IFileLoggingSession ^, LogFileGeneratedEventArgs ^>(this, &FileLoggingSessionScenario::OnLogFileGenerated);
    }

    // This sample adds the channel at level "warning" to 
    // demonstrated how messages logged at more verbose levels
    // are ignored by the session. 
    _session->AddLoggingChannel(_channel, LoggingLevel::Warning);
}

// Prepare this scenario for suspend. 
task<void> FileLoggingSessionScenario::PrepareToSuspendAsync()
{
    if (_session != nullptr)
    {
        _isPreparingForSuspend = true;
        // Before suspend, save any final log file.
        return create_task(CloseSessionSaveFinalLogFile()).then([this](StorageFile^ finalLogFile)
        {
            // Cleanup the session.
            delete _session;
            _session = nullptr;
            // Save values used when the app is resumed or started later.
            // Logging is enabled.
            SetAppLocalSettingsValue(LOGGING_ENABLED_SETTING_KEY_NAME, true);
            // If there's a final log file...
            if (finalLogFile != nullptr)
            {
                // Save the log file name saved at suspend so the sample UI can be 
                // updated on resume with that information. 
                SetAppLocalSettingsValue(LOGFILEGEN_BEFORE_SUSPEND_SETTING_KEY_NAME, finalLogFile->Path);
            }
            else
            {
                SetAppLocalSettingsValue(LOGFILEGEN_BEFORE_SUSPEND_SETTING_KEY_NAME, nullptr);
            }
            _isPreparingForSuspend = false;
            return task<void>([]() -> void {});
        });
    }
    else
    {
        // Save values used when the app is resumed or started later.
        // Logging is not enabled and no log file was saved.
        SetAppLocalSettingsValue(LOGGING_ENABLED_SETTING_KEY_NAME, false);
        SetAppLocalSettingsValue(LOGFILEGEN_BEFORE_SUSPEND_SETTING_KEY_NAME, nullptr);
        return task<void>([]() -> void {});
    }
}

// This is called when the app is either resuming or starting. 
// It will enable logging if the app has never been started before
// or if logging had been enabled the last time the app was running.
void FileLoggingSessionScenario::ResumeLoggingIfApplicable()
{
    bool loggingEnabled;
    if (IsAppLocalSettingsValue(LOGGING_ENABLED_SETTING_KEY_NAME))
    {
        loggingEnabled = GetAppLocalSettingsValueAsBool(LOGGING_ENABLED_SETTING_KEY_NAME);
    }
    else
    {         
        SetAppLocalSettingsValue(LOGGING_ENABLED_SETTING_KEY_NAME, true);
        loggingEnabled = true;
    }

    if (loggingEnabled)
    {
        StartLogging();
    }

    // When the sample suspends, it retains state as to whether or not it had
    // generated a new log file at the last suspension. This allows any
    // UI to be updated on resume to reflect that fact. 
    if (IsAppLocalSettingsValue(LOGFILEGEN_BEFORE_SUSPEND_SETTING_KEY_NAME))
    {
        Platform::String^ pathOfLogFileGeneratedBeforeSuspend = GetAppLocalSettingsValueAsString(LOGFILEGEN_BEFORE_SUSPEND_SETTING_KEY_NAME);
        if (pathOfLogFileGeneratedBeforeSuspend != nullptr)
        {
            StatusChanged(this, ref new LoggingScenarioEventArgs(LoggingScenarioEventType::LogFileGeneratedAtSuspend, pathOfLogFileGeneratedBeforeSuspend->Data()));
            SetAppLocalSettingsValue(LOGFILEGEN_BEFORE_SUSPEND_SETTING_KEY_NAME, nullptr);
        }
    }
}

// DoScenarioAsync creates a worker thread to simulate application activity.
// In actual use, logging functions would be called from various parts of a 
// larger application. The purpose of the below concise function is to show
// how logging works over periods of time when lots of messages are logged.
// Primarily, it shows how log files are generated when each log file reaches
// its maximum capacity. This function logs information in a loop 
// that exits after 3 log files have been generated. After each log file is 
// generated, the sample's UI is updated with the log file's name.
task<void> FileLoggingSessionScenario::DoScenarioAsync()
{
    // Tell the UI the scenario is busy working.
    IsBusy = true;
    try
    {
        // ContinuationData allows information to be
        // shared across the below async operations.
        struct ContinuationData
        {
            int messageIndex;
            int startFileCount;
            std::function<concurrency::task<void>(const std::shared_ptr<ContinuationData>&)> func;
        };

        // Create an instance of ContinuationData to share
        // across the below async tasks.
        auto data = std::make_shared<ContinuationData>();

        // A message index counter which increments for each
        // logging call. 
        data->messageIndex = 0;

        // Get the count of log files created so far. 
        // The code below will execute until 3 more 
        // log files are created.
        data->startFileCount = LogFileGeneratedCount;

        // This is the main worker function. It is 
        // called asynchronously until 3 new log files
        // have been created.
        data->func = [this](const std::shared_ptr<ContinuationData>& data)
        {
            const int NUMBER_OF_LOG_FILES_TO_GENERATE = 3;

            // To demo logging, loop and log messages until 3 log files have been produced.
            // Each time a new log is created, the FileLoggingSession will notify this sample
            // via the FileLoggingSession::LogFileGenerated event. 
            // While not 3 files yet, execute the following code...
            while (LogFileGeneratedCount - data->startFileCount < NUMBER_OF_LOG_FILES_TO_GENERATE)
            {
                // Since the channel is added to the session at level Warning, the following 
                // message is logged because it is logged at level LoggingLevel.Critical.
                Platform::String^ messageToLog =
                    "Message=" +
                    (++data->messageIndex).ToString() +
                    ": Lorem ipsum dolor sit amet, consectetur adipiscing elit.In ligula nisi, vehicula nec eleifend vel, rutrum non dolor.Vestibulum ante ipsum " +
                    "primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elementum scelerisque accumsan. In hac habitasse platea dictumst.";
                _channel->LogMessage(messageToLog, LoggingLevel::Critical);

                // Since the channel is added to the session at level Warning, the following 
                // message is *not* logged because it is logged at LoggingLevel.Information.
                messageToLog =
                    "Message=" +
                    (++data->messageIndex).ToString() +
                    ": Lorem ipsum dolor sit amet, consectetur adipiscing elit.In ligula nisi, vehicula nec eleifend vel, rutrum non dolor.Vestibulum ante ipsum " +
                    "primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elementum scelerisque accumsan. In hac habitasse platea dictumst.";
                this->_channel->LogMessage(messageToLog, LoggingLevel::Information);

                int value = 1000000; // one million, 7 digits, 4-bytes as an int, 14 bytes as a wide character string.
                _channel->LogMessage("Value #" + (++data->messageIndex).ToString() + "  " + value.ToString(), LoggingLevel::Critical); // value is logged as 14 byte wide character string.
                _channel->LogValuePair("Value #" + (++data->messageIndex).ToString(), value, LoggingLevel::Critical); // value is logged as a 4-byte integer.

                //
                // Pause every once in a while to simulate application activity outside of logging.
                //

                if (data->messageIndex % 50 == 0)
                {
                    // Create a task which calls this function again in 5 msecs.
                    // When this function is called again, it will continue its work.
                    return create_delayed_task(std::chrono::milliseconds(5), [=] { return data->func(data); });
                }
            }

            // 3 log files created, nothing more to do, stop.
            return task<void>([]() -> void {});
        };

        // Create the first task of several which will create 3 log files, 
        // and then execute the following continuation code when finished...
        return data->func(data).then([=](task<void> previousTask)
        {
            // This executes after 3 new log files have been created or
            // in the event of an unhandled exception in the above code.
            try
            {
                // Tell the sample UI that the scenario is not busy any longer.
                IsBusy = false;

                // The following 'get' will re-throw any exceptions which 
                // occurred during the above code (data->func above).
                previousTask.get(); 
            }
            catch (...)
            {
                OutputDebugString(L"Exception during DoScenario.");
                throw;
            }
        });
    }
    catch (...)
    {
        IsBusy = false;
        throw;
    }
}
