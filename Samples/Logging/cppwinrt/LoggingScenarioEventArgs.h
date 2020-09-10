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
#include "LoggingScenarioEventArgs.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct LoggingScenarioEventArgs : LoggingScenarioEventArgsT<LoggingScenarioEventArgs>
    {
        // e.g. for BusyStatusChanged
        LoggingScenarioEventArgs(LoggingScenarioEventType type)
            : _type(type) {}

        // e.g. for LogFileGenerated
        LoggingScenarioEventArgs(LoggingScenarioEventType type, hstring const& logFileFullPath)
            : _type(type), _logFileFullPath(logFileFullPath) {}

        // e.g. for LoggingEnabledDisabled
        LoggingScenarioEventArgs(LoggingScenarioEventType type, bool enabled)
            : _type(type), _enabled(enabled) {}

        LoggingScenarioEventType Type() const noexcept { return _type; }
        bool Enabled() const noexcept { return _enabled; }
        hstring LogFileFullPath() const noexcept { return _logFileFullPath; }

    private:
        LoggingScenarioEventType _type;
        bool _enabled = false;
        hstring _logFileFullPath;
    };
}
