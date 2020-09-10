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

#include "pch.h"
#include "FileLoggingSessionScenario.h"
#include "LoggingScenarioHelpers.h"

#define SCENARIO_PREFIX                     L"FileLoggingSessionScenario_"
#define DEFAULT_SESSION_NAME                SCENARIO_PREFIX L"Session"
#define DEFAULT_CHANNEL_NAME                SCENARIO_PREFIX L"Channel"
#define OUR_SAMPLE_APP_LOG_FILE_FOLDER_NAME SCENARIO_PREFIX L"LogFiles"
#define LOGGING_ENABLED_SETTING_KEY_NAME    SCENARIO_PREFIX L"LoggingEnabled"
#define LOGFILEGEN_BEFORE_SUSPEND_SETTING_KEY_NAME SCENARIO_PREFIX L"LogFileGeneratedBeforeSuspend"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Diagnostics;
using namespace winrt::Windows::Storage;
using namespace winrt::SDKTemplate;

FileLoggingSessionScenario::~FileLoggingSessionScenario()
{
}

FileLoggingSessionScenario::FileLoggingSessionScenario()
{
    // Create the logging channel.
    // When an app logs messages to a channel, the messges will go 
    // to any sessions which are consuming messages from the channel.
    _channel = LoggingChannel(DEFAULT_CHANNEL_NAME);

    // This sample listens to the channel's events so it can track 
    // information about the channel's enablement status. 
    _channel.LoggingEnabled({ this, &FileLoggingSessionScenario::OnChannelLoggingEnabled });

    // Attach event handlers for Suspend/Resume.
    Windows::UI::Xaml::Application app = Windows::UI::Xaml::Application::Current();
    app.Suspending({ this, &FileLoggingSessionScenario::OnAppSuspending });
    app.Resuming({ this, &FileLoggingSessionScenario::OnAppResuming });

    // If the app is being launched (not resumed), the following call will
    // activate logging if it had been active at the time of the last suspend.
    ResumeLoggingIfApplicable();
}

// static
FileLoggingSessionScenario&
FileLoggingSessionScenario::Instance()
{
    static FileLoggingSessionScenario instance;
    return instance;
}

event_token
FileLoggingSessionScenario::StatusChanged(StatusChangedHandler const& handler)
{
    return _statusChanged.add(handler);
}

void
FileLoggingSessionScenario::StatusChanged(event_token const& token) noexcept
{
    _statusChanged.remove(token);
}

bool
FileLoggingSessionScenario::IsBusy() const noexcept
{
    return _isBusy != 0;
}

int
FileLoggingSessionScenario::LogFileGeneratedCount() const noexcept
{
    return _logFileGeneratedCount;
}

bool
FileLoggingSessionScenario::LoggingEnabled() const noexcept
{
    return _session != nullptr;
}

// Handle a request to toggle logging enable/disable status.
IAsyncOperation<bool>
FileLoggingSessionScenario::ToggleLoggingEnabledDisabledAsync()
{
    BusySetter busy(*this);

    if (_session != nullptr)
    {
        // Save any final log file.
        StorageFile finalLogFile = co_await CloseSessionSaveFinalLogFile();

        // Cleanup the session and let the sample UI know of the new status.
        _session.Close();
        _session = nullptr;
        _statusChanged(make<implementation::LoggingScenarioEventArgs>(LoggingScenarioEventType::LogFileGeneratedAtDisable, finalLogFile == nullptr ? L"" : finalLogFile.Path()));
        _settings.Insert(LOGGING_ENABLED_SETTING_KEY_NAME, box_value(false));
        _statusChanged(make<implementation::LoggingScenarioEventArgs>(LoggingScenarioEventType::LoggingEnabledDisabled, false));
        co_return false;
    }
    else
    {
        // Enable logging and let the UI know of the new status.
        StartLogging();
        _settings.Insert(LOGGING_ENABLED_SETTING_KEY_NAME, box_value(true));
        _statusChanged(make<implementation::LoggingScenarioEventArgs>(LoggingScenarioEventType::LoggingEnabledDisabled, true));
        co_return true;
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
IAsyncAction
FileLoggingSessionScenario::DoScenarioAsync()
{
    // Tell the UI the scenario is busy working.
    BusySetter busy(*this);

    std::wstring messageToLog;

    int messageIndex = 0; // Increments for each logging call.
    int startFileCount = LogFileGeneratedCount();

    const int NUMBER_OF_LOG_FILES_TO_GENERATE = 3;

    // To demo logging, loop and log messages until 3 log files have been produced.
    // Each time a new log is created, the FileLoggingSession will notify this sample
    // via the FileLoggingSession::LogFileGenerated event. 
    // While not 3 files yet, execute the following code...
    while (LogFileGeneratedCount() - startFileCount < NUMBER_OF_LOG_FILES_TO_GENERATE)
    {
        // Since the channel is added to the session at level Warning, the following 
        // message is logged because it is logged at level LoggingLevel.Critical.
        messageToLog = L"Message=";
        messageToLog += std::to_wstring(++messageIndex);
        messageToLog +=
            L": Lorem ipsum dolor sit amet, consectetur adipiscing elit.In ligula nisi, vehicula nec eleifend vel, rutrum non dolor.Vestibulum ante ipsum "
            L"primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elementum scelerisque accumsan. In hac habitasse platea dictumst.";
        _channel.LogMessage(messageToLog, LoggingLevel::Critical);

        // Since the channel is added to the session at level Warning, the following 
        // message is *not* logged because it is logged at LoggingLevel.Information.
        messageToLog = L"Message=";
        messageToLog += std::to_wstring(++messageIndex);
        messageToLog +=
            L": Lorem ipsum dolor sit amet, consectetur adipiscing elit.In ligula nisi, vehicula nec eleifend vel, rutrum non dolor.Vestibulum ante ipsum "
            L"primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elementum scelerisque accumsan. In hac habitasse platea dictumst.";
        _channel.LogMessage(messageToLog, LoggingLevel::Information);

        int value = 1000000; // one million, 7 digits, 4-bytes as an int, 14 bytes as a wide character string.
        messageToLog = L"Value #";
        messageToLog += std::to_wstring(++messageIndex);
        messageToLog += L':';
        messageToLog += std::to_wstring(value);
        _channel.LogMessage(messageToLog, LoggingLevel::Critical); // 'value' is logged as 14 byte wide character string.
        ++messageIndex;
        _channel.LogValuePair(L"Value", value, LoggingLevel::Critical); // 'value' is logged as a 4-byte integer.

        //
        // Pause every once in a while to simulate application activity outside of logging.
        //

        if (messageIndex % 50 == 0)
        {
            co_await resume_after(std::chrono::milliseconds(5));
        }
    }
}

void
FileLoggingSessionScenario::StartLogging()
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
        _session = FileLoggingSession(DEFAULT_SESSION_NAME);
        _session.LogFileGenerated({ this, &FileLoggingSessionScenario::OnLogFileGenerated }); // event+=handler
    }

    // This sample adds the channel at level "warning" to 
    // demonstrated how messages logged at more verbose levels
    // are ignored by the session. 
    _session.AddLoggingChannel(_channel, LoggingLevel::Warning);
}

// Prepare this scenario for suspend. 
IAsyncAction
FileLoggingSessionScenario::PrepareToSuspendAsync()
{
    if (_session != nullptr)
    {
        _isPreparingForSuspend = true;

        // Before suspend, save any final log file.
        StorageFile finalLogFile = co_await CloseSessionSaveFinalLogFile();

        // Cleanup the session.
        _session.Close();
        _session = nullptr;

        // Save values used when the app is resumed or started later.
        // Logging is enabled.
        _settings.Insert(LOGGING_ENABLED_SETTING_KEY_NAME, box_value(true));

        // If there's a final log file...
        if (finalLogFile != nullptr)
        {
            // Save the log file name saved at suspend so the sample UI can be 
            // updated on resume with that information. 
            _settings.Insert(LOGFILEGEN_BEFORE_SUSPEND_SETTING_KEY_NAME, box_value(finalLogFile.Path()));
        }
        else
        {
            _settings.TryRemove(LOGFILEGEN_BEFORE_SUSPEND_SETTING_KEY_NAME);
        }

        _isPreparingForSuspend = false;
    }
    else
    {
        // Save values used when the app is resumed or started later.
        // Logging is not enabled and no log file was saved.
        _settings.Insert(LOGGING_ENABLED_SETTING_KEY_NAME, box_value(false));
        _settings.TryRemove(LOGFILEGEN_BEFORE_SUSPEND_SETTING_KEY_NAME);
    }
}

// This is called when the app is either resuming or starting. 
// It will enable logging if the app has never been started before
// or if logging had been enabled the last time the app was running.
void
FileLoggingSessionScenario::ResumeLoggingIfApplicable()
{
    bool loggingEnabled = unbox_value_or(_settings.TryLookup(LOGGING_ENABLED_SETTING_KEY_NAME), true);
    if (loggingEnabled)
    {
        StartLogging();
    }

    // When the sample suspends, it retains state as to whether or not it had
    // generated a new log file at the last suspension. This allows any
    // UI to be updated on resume to reflect that fact. 
    hstring pathOfLogFileGeneratedBeforeSuspend = unbox_value_or(_settings.TryLookup(LOGFILEGEN_BEFORE_SUSPEND_SETTING_KEY_NAME), L"");
    if (!pathOfLogFileGeneratedBeforeSuspend.empty())
    {
        _statusChanged(make<implementation::LoggingScenarioEventArgs>(LoggingScenarioEventType::LogFileGeneratedAtSuspend, pathOfLogFileGeneratedBeforeSuspend));
        _settings.TryRemove(LOGFILEGEN_BEFORE_SUSPEND_SETTING_KEY_NAME);
    }
}

IAsyncOperation<StorageFile>
FileLoggingSessionScenario::CloseSessionSaveFinalLogFile()
{
    // Tell the session to save its final log file.
    StorageFile finalFileBeforeSuspend = co_await _session.CloseAndSaveToFileAsync();

    if (finalFileBeforeSuspend != nullptr)
    {
        // A final log file was created.
        // Move it to the sample-defined log folder. 
        // First, get create/open our sample-defined log folder.
        StorageFolder ourSampleAppLogFolder =
            co_await ApplicationData::Current().LocalFolder().CreateFolderAsync(
                OUR_SAMPLE_APP_LOG_FILE_FOLDER_NAME,
                CreationCollisionOption::OpenIfExists);

        std::wstring logFileName;
        logFileName += L"Log-";
        logFileName += GetTimeStamp();
        logFileName += L".etl";
        co_await finalFileBeforeSuspend.MoveAsync(ourSampleAppLogFolder, logFileName, NameCollisionOption::FailIfExists);

        std::wstring fullPath;
        fullPath += ourSampleAppLogFolder.Path();
        fullPath += L'\\';
        fullPath += logFileName;

        StorageFile movedFile = co_await StorageFile::GetFileFromPathAsync(hstring(fullPath));
        return movedFile;
    }

    return nullptr;
}

void
FileLoggingSessionScenario::OnChannelLoggingEnabled(
    ILoggingChannel const& sender,
    IInspectable const&)
{
    // This method is called when the channel is informing us of channel-related state changes.
    // Save new channel state. These values can be used for advanced logging scenarios where, 
    // for example, it's desired to skip blocks of logging code if the channel is not being
    // consumed by any sessions. 
    _isChannelEnabled = sender.Enabled();
    _channelLoggingLevel = sender.Level();
}

// When the log file gets large, the system closes it and starts a new one.
// The LogFileGenerated event is raised to give the app a chance to move
// the recently-generated log file to a safe place. When the handler returns,
// the FileLoggingSession may reuse the file name for a new log file.
// This function moves the log file to an app-defined location.
fire_and_forget
FileLoggingSessionScenario::OnLogFileGenerated(
    IFileLoggingSession const& sender,
    LogFileGeneratedEventArgs const& args)
{
    ++_logFileGeneratedCount;

    StorageFile generatedLogFile = args.File();

    // Create the app-defined folder under the app's local folder.
    // An app defines where/how it wants to store log files. 
    // This sample uses OUR_SAMPLE_APP_LOG_FILE_FOLDER_NAME under
    // the app's local folder. 
    StorageFolder ourSampleAppLogFolder =
        co_await ApplicationData::Current().LocalFolder().CreateFolderAsync(
            OUR_SAMPLE_APP_LOG_FILE_FOLDER_NAME,
            CreationCollisionOption::OpenIfExists);

    std::wstring logFileName;
    logFileName += L"Log-";
    logFileName += GetTimeStamp();
    logFileName += L".etl";

    // Move the log file to our sample app's log folder. 
    co_await generatedLogFile.MoveAsync(ourSampleAppLogFolder, logFileName, NameCollisionOption::FailIfExists);

    // Update the UI to show the new log file.
    if (_isPreparingForSuspend == false)
    {
        hstring fullPath = ourSampleAppLogFolder.Path() + L"\\" + logFileName;
        _statusChanged(make<implementation::LoggingScenarioEventArgs>(LoggingScenarioEventType::LogFileGenerated, fullPath));
    }
}

fire_and_forget
FileLoggingSessionScenario::OnAppSuspending(
    IInspectable const&,
    Windows::ApplicationModel::SuspendingEventArgs const& e)
{
    // Get a deferral before performing any async operations
    // to avoid suspension prior to LoggingScenario completing 
    // PrepareToSuspendAsync().
    auto deferral = e.SuspendingOperation().GetDeferral();
    co_await PrepareToSuspendAsync();
    deferral.Complete();
}

void FileLoggingSessionScenario::OnAppResuming(
    IInspectable const&,
    IInspectable const&)
{
    // If logging was active at the last suspend,
    // ResumeLoggingIfApplicable will re-activate 
    // logging.
    ResumeLoggingIfApplicable();
}

FileLoggingSessionScenario::BusySetter::~BusySetter()
{
    if (0 == --_scenario._isBusy)
    {
        _scenario._statusChanged(make<implementation::LoggingScenarioEventArgs>(LoggingScenarioEventType::BusyStatusChanged));
    }
}

FileLoggingSessionScenario::BusySetter::BusySetter(FileLoggingSessionScenario& scenario)
    : _scenario(scenario)
{
    if (1 == ++_scenario._isBusy)
    {
        _scenario._statusChanged(make<implementation::LoggingScenarioEventArgs>(LoggingScenarioEventType::BusyStatusChanged));
    }
}
