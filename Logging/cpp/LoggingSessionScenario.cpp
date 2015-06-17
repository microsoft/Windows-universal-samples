//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
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

using namespace concurrency;
using namespace Windows::Foundation::Diagnostics;
using namespace Windows::Storage;
using namespace SDKTemplate;

LoggingSessionScenario^ LoggingSessionScenario::_instance;

LoggingSessionScenario::LoggingSessionScenario()
{
    _logFileGeneratedCount = 0;
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
    _channel->LoggingEnabled += ref new Windows::Foundation::TypedEventHandler<Windows::Foundation::Diagnostics::ILoggingChannel ^, Platform::Object ^>(this, &LoggingSessionScenario::OnChannelLoggingEnabled);
    App::Current->Suspending += ref new Windows::UI::Xaml::SuspendingEventHandler(this, &LoggingSessionScenario::OnAppSuspending);
    App::Current->Resuming += ref new Windows::Foundation::EventHandler<Platform::Object ^>(this, &LoggingSessionScenario::OnAppResuming);

    // If the app is being launched (not resumed), the
    // following call will activate logging if it had been
    // active at the time of the last suspend.
    ResumeLoggingIfApplicable();
}

LoggingSessionScenario::~LoggingSessionScenario()
{

}

void LoggingSessionScenario::OnAppSuspending(Platform::Object ^sender, Windows::ApplicationModel::SuspendingEventArgs ^e)
{
    (void) sender;    // Unused parameter
    auto deferral = e->SuspendingOperation->GetDeferral();
    PrepareToSuspend();
    deferral->Complete();
}

void LoggingSessionScenario::OnAppResuming(Platform::Object ^sender, Platform::Object ^args)
{
    // If logging was active at the last suspend,
    // ResumeLoggingIfApplicable will re-activate
    // logging.
    ResumeLoggingIfApplicable();
}

void LoggingSessionScenario::OnChannelLoggingEnabled(ILoggingChannel^ sender, Platform::Object^ args)
{
    // This method is called when the channel is informing us of channel-related state changes.
    // Save new channel state. These values can be used for advanced logging scenarios where,
    // for example, it's desired to skip blocks of logging code if the channel is not being
    // consumed by any sessions.
    _isChannelEnabled = sender->Enabled;
    _channelLoggingLevel = sender->Level;
}

task<StorageFile^> LoggingSessionScenario::SaveLogInMemoryToFileAsync()
{
    // Create the app-defined folder under the app's local folder.
    // An app defines where/how it wants to store log files.
    // This sample uses OUR_SAMPLE_APP_LOG_FILE_FOLDER_NAME under
    // the app's local folder.
    return create_task(ApplicationData::Current->LocalFolder->CreateFolderAsync(OUR_SAMPLE_APP_LOG_FILE_FOLDER_NAME, CreationCollisionOption::OpenIfExists))
        .then([this](StorageFolder^ ourSampleAppLogFolder)
        {
            // Create a new log file name.
            Platform::String^ newLogFileName = "Log-" + ref new Platform::String(GetTimeStamp().c_str()) + ".etl";
            // Asynchronously save the in-memory log file to a log file in our app's log folder.
            return create_task(this->_session->SaveToFileAsync(ourSampleAppLogFolder, newLogFileName));
        })
        .then([this](Windows::Storage::StorageFile^ savedLogFile)
        {
            // Return the log file as a StorageFile.
            return savedLogFile;
        });
}

bool LoggingSessionScenario::ToggleLoggingEnabledDisabled()
{
    IsBusy = true;
    try
    {
        if (_session != nullptr)
        {
            delete _session;
            _session = nullptr;
            SetAppLocalSettingsValue(LOGGING_ENABLED_SETTING_KEY_NAME, false);
            StatusChanged(this, ref new LoggingScenarioEventArgs(false));
            IsBusy = false;
            return false;
        }
        else
        {
            StartLogging();
            SetAppLocalSettingsValue(LOGGING_ENABLED_SETTING_KEY_NAME, true);
            StatusChanged(this, ref new LoggingScenarioEventArgs(true));
            IsBusy = false;
            return true;
        }
    }
    catch (...)
    {
        IsBusy = false;
        throw;
    }
}

void LoggingSessionScenario::StartLogging()
{
    // If no session exists, create one.
    // NOTE: There are use cases where an application
    // may want to create only a channel for sessions outside
    // of the application itself. See MSDN for details. This
    // sample is the common scenario of an app logging events
    // which it wants to place in its own log file, so it creates
    // a session and channel as a pair. The channel is created
    // during construction of this LoggingSessionScenario class so
    // it already exsits by the time this function is called.
    if (_session == nullptr)
    {
        _session = ref new Windows::Foundation::Diagnostics::LoggingSession(DEFAULT_SESSION_NAME);
    }

    // This sample adds the channel at level "warning" to
    // demonstrated how messages logged at more verbose levels
    // are ignored by the session.
    _session->AddLoggingChannel(_channel, LoggingLevel::Warning);
}

void LoggingSessionScenario::PrepareToSuspend()
{
    SetAppLocalSettingsValue(LOGGING_ENABLED_SETTING_KEY_NAME, _session != nullptr);
}

void LoggingSessionScenario::ResumeLoggingIfApplicable()
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
}

task<void> LoggingSessionScenario::DoScenarioAsync()
{
    IsBusy = true;
    try
    {
        // ContinuationData allows information to be
        // shared across the below async operations.
        struct ContinuationData
        {
            int messageIndex;
            int lastDelay;
            int lastAppError;
            int startFileCount;
            std::function<concurrency::task<void>(const std::shared_ptr<ContinuationData>&)> func;
        };

        // Create an instance of ContinuationData to share
        // across the below async tasks.
        auto data = std::make_shared<ContinuationData>();

        // A message index counter which increments for each
        // logging call.
        data->messageIndex = 0;

        // The messageIndex value at the last delay.
        data->lastDelay = 0;

        // The messageIndex value at the last simulated error.
        data->lastAppError = 0;

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

            while (LogFileGeneratedCount - data->startFileCount < NUMBER_OF_LOG_FILES_TO_GENERATE)
            {
                try
                {
                    // Since the channel is added to the session at level Warning,
                    // the following is logged because it is logged at level LoggingLevel.Critical.
                    Platform::String^ messageToLog =
                        "Message=" +
                        (++data->messageIndex).ToString() +
                        ": Lorem ipsum dolor sit amet, consectetur adipiscing elit.In ligula nisi, vehicula nec eleifend vel, rutrum non dolor.Vestibulum ante ipsum " +
                        "primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elementum scelerisque accumsan. In hac habitasse platea dictumst.";
                    _channel->LogMessage(messageToLog, LoggingLevel::Critical);

                    // Since the channel is added to the session at level Warning,
                    // the following is *not* logged because it is logged at LoggingLevel.Information.
                    messageToLog =
                        "Message=" +
                        (++data->messageIndex).ToString() +
                        ": Lorem ipsum dolor sit amet, consectetur adipiscing elit.In ligula nisi, vehicula nec eleifend vel, rutrum non dolor.Vestibulum ante ipsum " +
                        "primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elementum scelerisque accumsan. In hac habitasse platea dictumst.";
                    _channel->LogMessage(messageToLog, LoggingLevel::Information);

                    int value = 1000000; // one million, 7 digits, 4-bytes as an int, 14 bytes as a wide character string.
                    data->messageIndex++;
                    _channel->LogMessage("Value #" + data->messageIndex.ToString() + ":" + value.ToString(), LoggingLevel::Critical); // 'value' is logged as 14 byte wide character string.
                    data->messageIndex++;
                    _channel->LogValuePair("Value", value, LoggingLevel::Critical); // 'value' is logged as a 4-byte integer.

                    //
                    // Every once in a while, simulate an application error
                    // which causes the app to save the current snapshot
                    // of logging events in memory to a disk ETL file.
                    //

                    if (data->messageIndex - data->lastAppError >= 25000)
                    {
                        data->lastAppError = data->messageIndex;

                        // Before simulating an application error, demonstrate LoggingActivity.
                        // A LoggingActivity outputs a pair of begin and end messages to the channel.
                        // The first message is logged when the LoggingActivity is created. When
                        // exiting the scope of the LoggingActivity, the second message will be logged.
                        {
                            LoggingActivity activity("Add two numbers.", _channel, LoggingLevel::Critical);
                            int oneNumber = 100;
                            int anotherNumber = 200;
                            int total = oneNumber + anotherNumber;
                            _channel->LogMessage("Message=" + (++data->messageIndex).ToString() + ": The result of adding two numbers: " + total.ToString(), LoggingLevel::Critical);
                        } // The LoggingActivity instance will log the second message at this scope exit.

                        // Simulate an application error.
                        throw std::exception("Some bad app error occurred.");
                    }

                    //
                    // Pause every once in a while to simulate application activity outside of logging.
                    //

                    if (data->messageIndex - data->lastDelay >= 100)
                    {
                        data->lastDelay = data->messageIndex;

                        // Create a task which calls this function again in 5 msecs.
                        // When this function is called again, it will continue its work.
                        return create_delayed_task(std::chrono::milliseconds(5), [=]{ return data->func(data); });
                    }
                }
                catch (std::exception e)
                {
                    // Save in-memory logging buffer to a log file on error.

                    ::std::wstring wideWhat;
                    if (e.what() != nullptr)
                    {
                        int convertResult = MultiByteToWideChar(CP_UTF8, 0, e.what(), (int)strlen(e.what()), NULL, 0);
                        if (convertResult <= 0)
                        {
                            wideWhat = L"Exception occurred: Failure to convert its message text using MultiByteToWideChar: convertResult=";
                            wideWhat += convertResult.ToString()->Data();
                            wideWhat += L"  GetLastError()=";
                            wideWhat += GetLastError().ToString()->Data();
                        }
                        else
                        {
                            wideWhat.resize(convertResult + 10);
                            convertResult = MultiByteToWideChar(CP_UTF8, 0, e.what(), (int)strlen(e.what()), &wideWhat[0], (int)wideWhat.size());
                            if (convertResult <= 0)
                            {
                                wideWhat = L"Exception occurred: Failure to convert its message text using MultiByteToWideChar: convertResult=";
                                wideWhat += convertResult.ToString()->Data();
                                wideWhat += L"  GetLastError()=";
                                wideWhat += GetLastError().ToString()->Data();
                            }
                            else
                            {
                                wideWhat.insert(0, L"Exception occurrred: ");
                            }
                        }
                    }
                    else
                    {
                        wideWhat = L"Exception occurred: Unknown.";
                    }

                    Platform::String^ errorMessage = ref new Platform::String(wideWhat.c_str());
                    // The session added the channel at level Warning. Log the message at
                    // level Error which is above (more critical than) Warning, which
                    // means it will actually get logged.
                    _channel->LogMessage(errorMessage, LoggingLevel::Error);
                    SaveLogInMemoryToFileAsync().then([=](StorageFile^ logFile) {
                        _logFileGeneratedCount++;
                        StatusChanged(this, ref new LoggingScenarioEventArgs(LoggingScenarioEventType::LogFileGenerated, logFile->Path->Data()));
                    }).wait();
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
