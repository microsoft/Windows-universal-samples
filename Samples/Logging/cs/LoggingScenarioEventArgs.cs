//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

using System;

namespace SDKTemplate
{
    /// <summary>
    /// LoggingScenario tells the UI what's happening by 
    /// using the following enums. 
    /// </summary>
    enum LoggingScenarioEventType
    {
        BusyStatusChanged,
        LogFileGenerated,
        LogFileGeneratedAtDisable,
        LogFileGeneratedAtSuspend,
        LoggingEnabledDisabled
    }

    class LoggingScenarioEventArgs : EventArgs
    {
        public LoggingScenarioEventArgs(LoggingScenarioEventType type)
        {
            Type = type;
        }

        public LoggingScenarioEventArgs(LoggingScenarioEventType type, string logFilePath)
        {
            Type = type;
            LogFilePath = logFilePath;
        }

        public LoggingScenarioEventArgs(bool enabled)
        {
            Type = LoggingScenarioEventType.LoggingEnabledDisabled;
            Enabled = enabled;
        }

        public LoggingScenarioEventType Type { get; private set; }
        public string LogFilePath { get; private set; }
        public bool Enabled { get; private set; }
    }
}
