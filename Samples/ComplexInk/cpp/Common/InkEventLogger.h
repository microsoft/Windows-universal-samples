// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Resources.h"

class InkEventLogger
{
public:
    InkEventLogger();
    void SetConfiguration(LogEventTypes config);
    void EnableLoggingConfiguration(LogEventTypes type, bool addConfig);
    void Log(LogEventAction eventAction);
    std::wstring GetLog() { return _log; };
    void ClearLog() { _log = L""; };

private:
    LogEventTypes   _currentConfig;
    std::wstring    _log;

    unsigned long _countOfCoalescedIndependentInput;
    unsigned long _countOfCoalescedStrokeMessages;
    unsigned long _countOfCoalescedUnprocessedInput;
};

