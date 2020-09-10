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

namespace winrt::SDKTemplate
{
    class LoggingChannelScenario
    {
    public:

        void
        LogWithWin81Constructor();

        void
        LogWithWin10Constructor();

    private:

        void
        DemonstrateLogging(
            Windows::Foundation::Diagnostics::LoggingChannel const& channel);

        hstring
        CollectExpensiveData();

        void
        DoSomething(
            Windows::Foundation::Diagnostics::ILoggingTarget const& logger);

        void
        OnLoggingEnabled(
            Windows::Foundation::Diagnostics::ILoggingChannel const& sender,
            Windows::Foundation::IInspectable const& e);
    };
}
