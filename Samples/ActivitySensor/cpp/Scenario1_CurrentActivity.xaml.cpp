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

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Devices::Enumeration;
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

    // Determine if we can access activity sensors
    auto deviceAccessInfo = DeviceAccessInformation::CreateFromDeviceClassId(GUID_ActivitySensor_ClassId);
    if (deviceAccessInfo->CurrentStatus == DeviceAccessStatus::Allowed)
    {
        // Get the default activity sensor
        create_task(ActivitySensor::GetDefaultAsync()).then([this](ActivitySensor^ activitySensor)
        {
            if (activitySensor != nullptr)
            {
                create_task(activitySensor->GetCurrentReadingAsync()).then([this](task<ActivitySensorReading^> task)
                {
                    try
                    {
                        auto reading = task.get();
                        if (reading != nullptr)
                        {
                            ScenarioOutput_Activity->Text = reading->Activity.ToString();
                            ScenarioOutput_Confidence->Text = reading->Confidence.ToString();

                            auto timestampFormatter = ref new DateTimeFormatter("day month year hour minute second");
                            ScenarioOutput_Timestamp->Text = timestampFormatter->Format(reading->Timestamp);
                        }
                    }
                    catch (Platform::COMException ^ e)
                    {
                        // Some activity sensors may occassionally time out when determining the initial
                        // reading. If this happens frequently, use events instead of GetCurrentReadingAsync.
                        rootPage->NotifyUser("Could not get the current reading (likely a timeout)", NotifyType::ErrorMessage);
                    }
                });
            }
            else
            {
                rootPage->NotifyUser("No activity sensors found", NotifyType::ErrorMessage);
            }
        });
    }
    else
    {
        rootPage->NotifyUser("Access denied to activity sensors", NotifyType::ErrorMessage);
    }
}

