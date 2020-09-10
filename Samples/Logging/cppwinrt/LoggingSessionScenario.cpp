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
#include "LoggingSessionScenario.h"
#include "LoggingScenarioHelpers.h"

#define SCENARIO_PREFIX                     L"LoggingSessionScenario_"
#define DEFAULT_SESSION_NAME                SCENARIO_PREFIX L"Session"
#define DEFAULT_CHANNEL_NAME                SCENARIO_PREFIX L"Channel"
#define OUR_SAMPLE_APP_LOG_FILE_FOLDER_NAME SCENARIO_PREFIX L"LogFiles"
#define LOGGING_ENABLED_SETTING_KEY_NAME    SCENARIO_PREFIX L"LoggingEnabled"

using namespace winrt;
using namespace winrt::Windows::ApplicationModel;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Diagnostics;
using namespace winrt::Windows::Storage;
using namespace winrt::SDKTemplate;

LoggingSessionScenario::~LoggingSessionScenario()
{
}

LoggingSessionScenario::LoggingSessionScenario()
{
    // Create the logging channel.
    // When an app logs messages to a channel, the messges will go
    // to any sessions which are consuming messages from the channel.
    _channel = LoggingChannel(DEFAULT_CHANNEL_NAME);

    // This sample listens to the channel's events so it can track
    // information about the channel's enablement status.
    _channel.LoggingEnabled({ this, &LoggingSessionScenario::OnChannelLoggingEnabled });

    // Attach event handlers for Suspend/Resume.
    Windows::UI::Xaml::Application app = Windows::UI::Xaml::Application::Current();
    app.Suspending({ this, &LoggingSessionScenario::OnAppSuspending });
    app.Resuming({ this, &LoggingSessionScenario::OnAppResuming });

    // If the app is being launched (not resumed), the following call will
    // activate logging if it had been active at the time of the last suspend.
    ResumeLoggingIfApplicable();
}

// static
LoggingSessionScenario&
LoggingSessionScenario::Instance()
{
    static LoggingSessionScenario instance;
    return instance;
}

event_token
LoggingSessionScenario::StatusChanged(StatusChangedHandler const& handler)
{
    return _statusChanged.add(handler);
}

void
LoggingSessionScenario::StatusChanged(event_token const& token) noexcept
{
    _statusChanged.remove(token);
}

bool
LoggingSessionScenario::IsBusy() const noexcept
{
    return _isBusy != 0;
}

int
LoggingSessionScenario::LogFileGeneratedCount() const noexcept
{
    return _logFileGeneratedCount;
}

bool
LoggingSessionScenario::LoggingEnabled() const noexcept
{
    return _session != nullptr;
}

bool
LoggingSessionScenario::ToggleLoggingEnabledDisabled()
{
    BusySetter busy(*this);

    if (_session != nullptr)
    {
        _session.Close();
        _session = nullptr;
        _settings.Insert(LOGGING_ENABLED_SETTING_KEY_NAME, box_value(false));
        _statusChanged(make<implementation::LoggingScenarioEventArgs>(LoggingScenarioEventType::LoggingEnabledDisabled, false));
        return false;
    }
    else
    {
        StartLogging();
        _settings.Insert(LOGGING_ENABLED_SETTING_KEY_NAME, box_value(true));
        _statusChanged(make<implementation::LoggingScenarioEventArgs>(LoggingScenarioEventType::LoggingEnabledDisabled, true));
        return true;
    }
}

IAsyncAction
LoggingSessionScenario::DoScenarioAsync()
{
    BusySetter busy(*this);

    std::wstring messageToLog;

    int messageIndex = 0; // Increments for each logging call.
    int lastDelayIndex = 0; // The messageIndex value at the last delay.
    int lastAppErrorIndex = 0; // The messageIndex value at the last simulated error.
    int startFileCount = LogFileGeneratedCount();

    const int NUMBER_OF_LOG_FILES_TO_GENERATE = 3;

    while (LogFileGeneratedCount() - startFileCount < NUMBER_OF_LOG_FILES_TO_GENERATE)
    {
        bool exceptionOccurred = false;
        try
        {
            // Since the channel is added to the session at level Warning,
            // the following is logged because it is logged at level LoggingLevel.Critical.
            messageToLog = L"Message=";
            messageToLog += std::to_wstring(++messageIndex);
            messageToLog +=
                L": Lorem ipsum dolor sit amet, consectetur adipiscing elit.In ligula nisi, vehicula nec eleifend vel, rutrum non dolor.Vestibulum ante ipsum "
                L"primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elementum scelerisque accumsan. In hac habitasse platea dictumst.";
            _channel.LogMessage(messageToLog, LoggingLevel::Critical);

            // Since the channel is added to the session at level Warning,
            // the following is *not* logged because it is logged at LoggingLevel.Information.
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
            // Every once in a while, simulate an application error
            // which causes the app to save the current snapshot
            // of logging events in memory to a disk ETL file.
            //

            if (messageIndex - lastAppErrorIndex >= 25000)
            {
                lastAppErrorIndex = messageIndex;

                // Before simulating an application error, demonstrate LoggingActivity.
                // A LoggingActivity outputs a pair of begin and end messages to the channel.
                // The first message is logged when the LoggingActivity is created. When
                // exiting the scope of the LoggingActivity, the second message will be logged.
                {
                    LoggingActivity activity(L"Add two numbers.", _channel,
                        LoggingLevel::Critical);
                    int oneNumber = 100;
                    int anotherNumber = 200;
                    int total = oneNumber + anotherNumber;
                    messageToLog = L"Message=";
                    messageToLog += std::to_wstring(++messageIndex);
                    messageToLog += L": The result of adding two numbers: ";
                    messageToLog += std::to_wstring(total);
                    _channel.LogMessage(messageToLog, LoggingLevel::Critical);
                } // The LoggingActivity instance will log the end message at this scope exit.

                // Simulate an application error.
                throw std::exception("Some bad app error occurred.");
            }

            //
            // Pause every once in a while to simulate application activity outside of logging.
            //

            if (messageIndex - lastDelayIndex >= 100)
            {
                lastDelayIndex = messageIndex;
                co_await resume_after(std::chrono::milliseconds(5));
            }
        }
        catch (std::exception const& e)
        {
            // Save in-memory logging buffer to a log file on error.

            hstring what;
            if (e.what() != nullptr)
            {
                what = to_hstring(e.what());
            }
            else
            {
                what = L"Unknown.";
            }

            // The session added the channel at level Warning. Log the message at
            // level Error which is above (more critical than) Warning, which
            // means it will actually get logged.
            _channel.LogMessage(L"Exception occurred: " + what, LoggingLevel::Error);

            // Since co_await is not supported within catch blocks, record the exception.
            // After the catch block, we'll notice that an exception occurred and save the log.
            exceptionOccurred = true;
        }

        if (exceptionOccurred)
        {
            // An exception occurred, so save the in-memory events to disk for analysis.

            // Create the app-defined folder under the app's local folder.
            // An app defines where/how it wants to store log files.
            // This sample uses OUR_SAMPLE_APP_LOG_FILE_FOLDER_NAME under
            // the app's local folder.
            StorageFolder ourSampleAppLogFolder =
                co_await ApplicationData::Current().LocalFolder().CreateFolderAsync(
                    OUR_SAMPLE_APP_LOG_FILE_FOLDER_NAME,
                    CreationCollisionOption::OpenIfExists);

            // Create a new log file name.
            std::wstring newLogFileName;
            newLogFileName += L"Log-";
            newLogFileName += GetTimeStamp();
            newLogFileName += L".etl";

            StorageFile logFile = co_await _session.SaveToFileAsync(ourSampleAppLogFolder, newLogFileName);
            ++_logFileGeneratedCount;
            _statusChanged(make<implementation::LoggingScenarioEventArgs>(LoggingScenarioEventType::LogFileGenerated, logFile.Path()));
        }
    }
}

void
LoggingSessionScenario::StartLogging()
{
    // If no session exists, create one.
    // NOTE: There are use cases where an application
    // may want to create only a channel for sessions outside
    // of the application itself. See MSDN for details. This
    // sample is the common scenario of an app logging events
    // which it wants to place in its own log file, so it creates
    // a session and channel as a pair. The channel is created
    // during construction of this LoggingSessionScenario class so
    // it already exists by the time this function is called.
    if (_session == nullptr)
    {
        _session = LoggingSession(DEFAULT_SESSION_NAME);
    }

    // This sample adds the channel at level "warning" to
    // demonstrate how messages logged at more verbose levels
    // are ignored by the session.
    _session.AddLoggingChannel(_channel, LoggingLevel::Warning);
}

void
LoggingSessionScenario::PrepareToSuspend()
{
    _settings.Insert(LOGGING_ENABLED_SETTING_KEY_NAME, box_value(_session != nullptr));
}

void
LoggingSessionScenario::ResumeLoggingIfApplicable()
{
    bool loggingEnabled = unbox_value_or(_settings.TryLookup(LOGGING_ENABLED_SETTING_KEY_NAME), true);
    if (loggingEnabled)
    {
        StartLogging();
    }
}

void
LoggingSessionScenario::OnChannelLoggingEnabled(
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

void
LoggingSessionScenario::OnAppSuspending(
    IInspectable const&,
    SuspendingEventArgs const& e)
{
    auto deferral = e.SuspendingOperation().GetDeferral();
    PrepareToSuspend();
    deferral.Complete();
}

void
LoggingSessionScenario::OnAppResuming(
    IInspectable const&,
    IInspectable const&)
{
    // If logging was active at the last suspend, ResumeLoggingIfApplicable
    // will re-activate logging.
    ResumeLoggingIfApplicable();
}

LoggingSessionScenario::BusySetter::~BusySetter()
{
    if (0 == --_scenario._isBusy)
    {
        _scenario._statusChanged(make<implementation::LoggingScenarioEventArgs>(LoggingScenarioEventType::BusyStatusChanged));
    }
}

LoggingSessionScenario::BusySetter::BusySetter(LoggingSessionScenario& scenario)
    : _scenario(scenario)
{
    if (1 == ++_scenario._isBusy)
    {
        _scenario._statusChanged(make<implementation::LoggingScenarioEventArgs>(LoggingScenarioEventType::BusyStatusChanged));
    }
}
