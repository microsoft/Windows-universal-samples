//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#pragma once
#include "LoggingScenarioEventArgs.h"

namespace SDKTemplate
{
    ref class FileLoggingSessionScenario sealed
    {
    internal:

        event StatusChangedHandler^ StatusChanged;

        static property FileLoggingSessionScenario^ Instance
        {
            FileLoggingSessionScenario^ get()
            {
                if (_instance == nullptr)
                {
                    _instance = ref new FileLoggingSessionScenario();
                }
                return _instance;
            }
        }

        property bool IsBusy
        {
            bool get() { return _isBusy; }

        private: 

            void set(bool value)
            {
                _isBusy = value;
                StatusChanged(this, ref new LoggingScenarioEventArgs(LoggingScenarioEventType::BusyStatusChanged));
            }
        }

        property int LogFileGeneratedCount
        {
            int get() { return _logFileGeneratedCount; }
        }            

        property bool LoggingEnabled
        {
            bool get() { return _session != nullptr; }
        }

        concurrency::task<bool> ToggleLoggingEnabledDisabledAsync();
        void StartLogging();

        concurrency::task<void> PrepareToSuspendAsync();
        void ResumeLoggingIfApplicable();

        concurrency::task<void> DoScenarioAsync();

    private:

        FileLoggingSessionScenario();
        ~FileLoggingSessionScenario();

        static void SetAppLocalSettingsValue(Platform::String^ key, Platform::Object^ value)
        {
            if (Windows::Storage::ApplicationData::Current->LocalSettings->Values->HasKey(key))
            {
                Windows::Storage::ApplicationData::Current->LocalSettings->Values->Remove(key);
            }
            Windows::Storage::ApplicationData::Current->LocalSettings->Values->Insert(key, value);
        }

        static bool IsAppLocalSettingsValue(Platform::String^ key)
        {
            return Windows::Storage::ApplicationData::Current->LocalSettings->Values->HasKey(key);
        }

        static Platform::Object^ GetAppLocalSettingsValueAsObject(Platform::String^ key)
        {
            return Windows::Storage::ApplicationData::Current->LocalSettings->Values->Lookup(key);
        }

        static bool GetAppLocalSettingsValueAsBool(Platform::String^ key)
        {
            Platform::Object^ obj = GetAppLocalSettingsValueAsObject(key);
            return safe_cast<bool>(obj);
        }

        static Platform::String^ GetAppLocalSettingsValueAsString(Platform::String^ key)
        {
            Platform::Object^ obj = GetAppLocalSettingsValueAsObject(key);
            return safe_cast<Platform::String^>(obj);
        }

        static FileLoggingSessionScenario^ _instance;
        Windows::Foundation::Diagnostics::FileLoggingSession^ _session;
        Windows::Foundation::Diagnostics::LoggingChannel^ _channel;

        long _logFileGeneratedCount;
        bool _isPreparingForSuspend;
        bool _isBusy;

        // The following are set when a LoggingChannel 
        // calls OnChannelLoggingEnabled:
        bool _isChannelEnabled;
        Windows::Foundation::Diagnostics::LoggingLevel _channelLoggingLevel;
        Windows::Foundation::EventRegistrationToken _onChannelLoggingEnabledToken;
        void OnChannelLoggingEnabled(Windows::Foundation::Diagnostics::ILoggingChannel ^sender, Platform::Object ^args);

        Windows::Foundation::EventRegistrationToken _onLogFileGeneratedToken;
        void OnLogFileGenerated(Windows::Foundation::Diagnostics::IFileLoggingSession^ sender, Windows::Foundation::Diagnostics::LogFileGeneratedEventArgs^ args);

        concurrency::task<Windows::Storage::StorageFile^> CloseSessionSaveFinalLogFile();
        void OnAppSuspending(Platform::Object ^sender, Windows::ApplicationModel::SuspendingEventArgs ^e);
        void OnAppResuming(Platform::Object ^sender, Platform::Object ^args);
    };
}
