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

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace Concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Create a bottom-up list", "SDKTemplate.Scenario1_Basic" },
    { "Incremental loading", "SDKTemplate.Scenario2_Load" },
};

String^ Helpers::GetCurrentDateTimeAsString()
{
    static auto calendar = ref new Windows::Globalization::Calendar();
    static auto formatter = ref new Windows::Globalization::DateTimeFormatting::DateTimeFormatter("shortdate longtime");
    calendar->SetToNow();
    return formatter->Format(calendar->GetDateTime());
}

TimeSpan Helpers::TimeSpanFromSeconds(int seconds)
{
    TimeSpan timeSpan;
    timeSpan.Duration = seconds * 1000LL * 10000LL; // convert seconds to TimeSpan units
    return timeSpan;
}

task<void> Helpers::DelayAsync(TimeSpan duration, Concurrency::cancellation_token cancel)
{
    task_completion_event<void> tce;
    ThreadPoolTimer^ timer = ThreadPoolTimer::CreateTimer(ref new TimerElapsedHandler([tce](ThreadPoolTimer^)
    {
        tce.set();
    }), duration);
    return task<void>(tce, cancel);
}