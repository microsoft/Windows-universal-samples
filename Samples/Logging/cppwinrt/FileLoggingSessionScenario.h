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

#pragma once
#include "LoggingScenarioEventArgs.h"

namespace winrt::SDKTemplate
{
    class FileLoggingSessionScenario
    {
        // This is a singleton, only created/destroyed by the Instance() method.
        ~FileLoggingSessionScenario();
        FileLoggingSessionScenario();

    public:

        static FileLoggingSessionScenario&
        Instance();

        event_token
        StatusChanged(StatusChangedHandler const& handler);

        void
        StatusChanged(event_token const& token) noexcept;

        bool
        IsBusy() const noexcept;

        int
        LogFileGeneratedCount() const noexcept;

        bool
        LoggingEnabled() const noexcept;

        Windows::Foundation::IAsyncOperation<bool>
        ToggleLoggingEnabledDisabledAsync();

        Windows::Foundation::IAsyncAction
        DoScenarioAsync();

    private:

        void
        StartLogging();

        Windows::Foundation::IAsyncAction
        PrepareToSuspendAsync();

        void
        ResumeLoggingIfApplicable();

        Windows::Foundation::IAsyncOperation<Windows::Storage::StorageFile>
        CloseSessionSaveFinalLogFile();

        void
        OnChannelLoggingEnabled(
            Windows::Foundation::Diagnostics::ILoggingChannel const& sender,
            Windows::Foundation::IInspectable const& args);

        fire_and_forget
        OnLogFileGenerated(
            Windows::Foundation::Diagnostics::IFileLoggingSession const& sender,
            Windows::Foundation::Diagnostics::LogFileGeneratedEventArgs const& args);

        fire_and_forget
        OnAppSuspending(Windows::Foundation::IInspectable const& sender,
            Windows::ApplicationModel::SuspendingEventArgs const& e);

        void
        OnAppResuming(Windows::Foundation::IInspectable const& sender,
            Windows::Foundation::IInspectable const& args);

    private:

        struct BusySetter
        {
            ~BusySetter();
            explicit BusySetter(FileLoggingSessionScenario& scenario);
            FileLoggingSessionScenario& _scenario;
        };

        Windows::Foundation::Diagnostics::FileLoggingSession _session{ nullptr };
        Windows::Foundation::Diagnostics::LoggingChannel _channel{ nullptr };

        std::atomic<int> _logFileGeneratedCount;
        std::atomic<int> _isBusy;
        bool _isPreparingForSuspend = false;

        bool _isChannelEnabled = false;
        Windows::Foundation::Diagnostics::LoggingLevel _channelLoggingLevel{};

        Windows::Foundation::Collections::IPropertySet _settings{ Windows::Storage::ApplicationData::Current().LocalSettings().Values() };

        event<StatusChangedHandler> _statusChanged;
    };
}
