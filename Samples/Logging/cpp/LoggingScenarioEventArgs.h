//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#pragma once
#include "pch.h"
#include <string>

namespace SDKTemplate
{
    // LoggingScenario tells the UI what's happening by 
    // using the following enums. 
    enum LoggingScenarioEventType
    {
        BusyStatusChanged,
        LogFileGenerated,
        LogFileGeneratedAtDisable,
        LogFileGeneratedAtSuspend,
        LoggingEnabledDisabled
    };

    ref class LoggingScenarioEventArgs sealed
    {
    private:

        LoggingScenarioEventType _type;
        std::wstring _logFileFullPath;
        bool _enabled;

    internal:

        LoggingScenarioEventArgs(LoggingScenarioEventType type)
        {
            _type = type;
        }

        LoggingScenarioEventArgs(LoggingScenarioEventType type, const std::wstring& logFileFullPath)
        {
            _type = type;
            _logFileFullPath = logFileFullPath;
        }

        LoggingScenarioEventArgs(bool enabled)
        {
            _type = LoggingEnabledDisabled;
            _enabled = enabled;
        }

        property LoggingScenarioEventType Type
        {
            LoggingScenarioEventType get()
            {
                return _type;
            }
        }

        property std::wstring LogFileFullPath
        {
            std::wstring get()
            {
                return _logFileFullPath;
            }
        }

        property bool Enabled
        {
            bool get()
            {
                return _enabled;
            }
        }
    };

    delegate void StatusChangedHandler(Platform::Object^ sender, LoggingScenarioEventArgs^ args);
}
