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

//
// Scenario1_CurrentActivity.xaml.cpp
// Implementation of the Scenario1_CurrentActivity class
//

#include "pch.h"
#include "Scenario1_CurrentActivity.xaml.h"

using namespace ActivitySensorCPP;
using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Scenario1_CurrentActivity::Scenario1_CurrentActivity() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario1_CurrentActivity::ScenarioGetCurrentActivity(Object^ sender, RoutedEventArgs^ e)
{
    // Reset fields and status
    ScenarioOutput_Activity->Text = "No data";
    ScenarioOutput_Confidence->Text = "No data";
    ScenarioOutput_Timestamp->Text = "No data";
    rootPage->NotifyUser("", NotifyType::StatusMessage);

    task<ActivitySensor^> getDefaultTask(ActivitySensor::GetDefaultAsync());
    getDefaultTask.then([this](task<ActivitySensor^> task)
    {
        try
        {
            ActivitySensor^ activitySensor = task.get();
            if (nullptr != activitySensor)
            {
                create_task(activitySensor->GetCurrentReadingAsync()).then([this](ActivitySensorReading^ reading)
                {
                    ScenarioOutput_Activity->Text = reading->Activity.ToString();
                    ScenarioOutput_Confidence->Text = reading->Confidence.ToString();

                    auto timestampFormatter = ref new DateTimeFormatter("day month year hour minute second");
                    ScenarioOutput_Timestamp->Text = timestampFormatter->Format(reading->Timestamp);
                });
            }
            else
            {
                rootPage->NotifyUser("No activity sensor found", NotifyType::ErrorMessage);
            }
        }
        catch (AccessDeniedException^)
        {
            rootPage->NotifyUser("User has denied access to the activity sensor", NotifyType::ErrorMessage);
        }
    });
}

