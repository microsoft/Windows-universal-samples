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
using namespace Windows::Data::Xml::Dom;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Windows::UI::Notifications;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Unspecified Reason", "SDKTemplate.UnspecifiedReason" },
    { "Saving Data Reason", "SDKTemplate.SavingDataReason" },
    { "Location Tracking Reason", "SDKTemplate.LocationTrackingReason" },
    { "Using Multiple Tasks", "SDKTemplate.MultipleTasks" },
};

ToastNotification^ MainPage::DisplayToast(String^ content)
{
    String^ xml = "<toast activationType='foreground'>"
        "<visual>"
            "<binding template='ToastGeneric'>"
                "<text>Extended Execution</text>"
            "</binding>"
        "</visual>"
    "</toast>";

    XmlDocument^ doc = ref new XmlDocument();
    doc->LoadXml(xml);

    auto binding = doc->SelectSingleNode("//binding");

    auto el = doc->CreateElement("text");
    el->InnerText = content;
    binding->AppendChild(el); // Add content to notification

    auto toast = ref new ToastNotification(doc);

    ToastNotificationManager::CreateToastNotifier()->Show(toast); // Show the toast

    return toast;
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
