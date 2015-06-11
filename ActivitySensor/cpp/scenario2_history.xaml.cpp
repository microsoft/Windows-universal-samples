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
// Scenario2_History.xaml.cpp
// Implementation of the Scenario2_History class
//

#include "pch.h"
#include "Scenario2_History.xaml.h"

using namespace ActivitySensorCPP;
using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Scenario2_History::Scenario2_History() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario2_History::ScenarioGetActivityHistory(Object^ sender, RoutedEventArgs^ e)
{
    // Reset fields and status
    ScenarioOutput_Count->Text = "No data";
    ScenarioOutput_Activity1->Text = "No data";
    ScenarioOutput_Confidence1->Text = "No data";
    ScenarioOutput_Timestamp1->Text = "No data";
    ScenarioOutput_ActivityN->Text = "No data";
    ScenarioOutput_ConfidenceN->Text = "No data";
    ScenarioOutput_TimestampN->Text = "No data";
    rootPage->NotifyUser("", NotifyType::StatusMessage);

    auto calendar = ref new Calendar();
    calendar->SetToNow();
    calendar->AddDays(-1);
    auto yesterday = calendar->GetDateTime();

    // Get history from yesterday onwards
    create_task(ActivitySensor::GetSystemHistoryAsync(yesterday)).then([this](IVectorView<ActivitySensorReading^>^ history)
    {
        ScenarioOutput_Count->Text = history->Size.ToString();
        if (history->Size > 0)
        {
            auto reading1 = history->GetAt(0);
            ScenarioOutput_Activity1->Text = reading1->Activity.ToString();
            ScenarioOutput_Confidence1->Text = reading1->Confidence.ToString();

            auto timestampFormatter = ref new DateTimeFormatter("day month year hour minute second");
            ScenarioOutput_Timestamp1->Text = timestampFormatter->Format(reading1->Timestamp);

            auto readingN = history->GetAt(history->Size - 1);
            ScenarioOutput_ActivityN->Text = readingN->Activity.ToString();
            ScenarioOutput_ConfidenceN->Text = readingN->Confidence.ToString();
            ScenarioOutput_TimestampN->Text = timestampFormatter->Format(readingN->Timestamp);
        }
    });
}

