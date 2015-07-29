//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#pragma once

namespace SDKTemplate
{
    ref class LoggingChannelScenario sealed
    {
    public:

        void LogWithWin81Constructor();
        void LogWithWin10Constructor();

    private:

        void DemonstrateLogging(Windows::Foundation::Diagnostics::LoggingChannel^ channel);
        Platform::String^ CollectExpensiveData();
        void DoSomething(Windows::Foundation::Diagnostics::ILoggingTarget^ logger);
        void OnLoggingEnabled(Windows::Foundation::Diagnostics::ILoggingChannel^ sender, Platform::Object^ args);
    };
}
