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
#include "Scenario3_CurrentStepCount.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

/// <summary>
/// CurrentStepCount page that the user is navigated to when 'Current step count' scenario is selected.
/// </summary>
Scenario3_CurrentStepCount::Scenario3_CurrentStepCount() : rootPage(MainPage::Current)
{
    InitializeComponent();

    // Disable buttons until we can access a pedometer
    GetCurrentButton->IsEnabled = false;

    // Determine if we can access pedometers
    auto deviceAccessInfo = DeviceAccessInformation::CreateFromDeviceClassId(GUID_Pedometer_ClassId);
    if (deviceAccessInfo->CurrentStatus == DeviceAccessStatus::Allowed)
    {
        // Determine if a pedometer is present
        // This can also be done using Windows::Devices::Enumeration::DeviceInformation::FindAllAsync
        create_task(Pedometer::GetDefaultAsync).then([this](task<Pedometer^> task)
        {
            pedometer = task.get();
            if (nullptr == pedometer)
            {
                rootPage->NotifyUser("No pedometers available", NotifyType::ErrorMessage);
            }
            else
            {
                rootPage->NotifyUser("Hit the 'Get steps count' Button", NotifyType::StatusMessage);
                GetCurrentButton->IsEnabled = true;
            }
        });
    }
}

/// <summary>
/// Invoked when 'GetCurrentButton' is clicked.
/// 'ReadingChanged' will not be fired when there is no activity on the pedometer 
/// and hence can't be reliably used to get the current step count. This handler makes
/// use of pedometer history on the system to get the current step count of the parameter
/// </summary>
/// <param name="sender">unused</param>
/// <param name="e">unused</param>
void Scenario3_CurrentStepCount::GetCurrentButton_Click(Platform::Object^ /*sender*/, Windows::UI::Xaml::RoutedEventArgs^ /*e*/)
{
    // Disable the button while we get the history
    GetCurrentButton->IsEnabled = false;

    auto currentReadings = pedometer->GetCurrentReadings();

    // To define the step goal, figure out the current step count
    auto totalStepCount = 0;
    bool updateTimestamp = true;
    for (auto kind = PedometerStepKind::Unknown; kind <= PedometerStepKind::Running; kind++)
    {
        if (currentReadings->HasKey(kind))
        {
            auto reading = currentReadings->Lookup(kind);
            totalStepCount += reading->CumulativeSteps;

            if (updateTimestamp)
            {
                auto timestampFormatter = ref new DateTimeFormatter("day month year hour minute second");
                ScenarioOutput_Timestamp->Text = timestampFormatter->Format(reading->Timestamp);
                updateTimestamp = false;
            }
        }
    }
    ScenarioOutput_TotalStepCount->Text = totalStepCount.ToString();

    // Re-enable the button
    GetCurrentButton->IsEnabled = true;
}
