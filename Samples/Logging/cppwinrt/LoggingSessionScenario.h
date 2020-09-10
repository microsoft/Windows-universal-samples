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
    class LoggingSessionScenario
    {
        // This is a singleton, only created/destroyed by the Instance() method.
        ~LoggingSessionScenario();
        LoggingSessionScenario();

    public:

        static LoggingSessionScenario&
        Instance();

        winrt::event_token
        StatusChanged(StatusChangedHandler const& handler);

        void
        StatusChanged(winrt::event_token const& token) noexcept;

        bool
        IsBusy() const noexcept;

        int
        LogFileGeneratedCount() const noexcept;

        bool
        LoggingEnabled() const noexcept;

        bool
        ToggleLoggingEnabledDisabled();

        Windows::Foundation::IAsyncAction
        DoScenarioAsync();

    private:

        void
        StartLogging();

        void
        PrepareToSuspend();

        void
        ResumeLoggingIfApplicable();

        void
        OnChannelLoggingEnabled(
            Windows::Foundation::Diagnostics::ILoggingChannel const& sender,
            Windows::Foundation::IInspectable const& args);

        void
        OnAppSuspending(
            Windows::Foundation::IInspectable const& sender,
            Windows::ApplicationModel::SuspendingEventArgs const& e);

        void
        OnAppResuming(
            Windows::Foundation::IInspectable const& sender,
            Windows::Foundation::IInspectable const& args);

    private:

        struct BusySetter
        {
            ~BusySetter();
            explicit BusySetter(LoggingSessionScenario& scenario);
            LoggingSessionScenario& _scenario;
        };

        Windows::Foundation::Diagnostics::LoggingSession _session{ nullptr };
        Windows::Foundation::Diagnostics::LoggingChannel _channel{ nullptr };

        std::atomic<int> _logFileGeneratedCount;
        std::atomic<int> _isBusy;

        bool _isChannelEnabled = false;
        Windows::Foundation::Diagnostics::LoggingLevel _channelLoggingLevel{};

        Windows::Foundation::Collections::IPropertySet _settings{ Windows::Storage::ApplicationData::Current().LocalSettings().Values() };

        event<StatusChangedHandler> _statusChanged;
    };
}
