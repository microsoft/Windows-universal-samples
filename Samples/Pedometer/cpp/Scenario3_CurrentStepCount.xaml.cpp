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
            auto pedometer = task.get();
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
    DateTime fromBeginning = {};

    // Disable the button while we get the history
    GetCurrentButton->IsEnabled = false;

    rootPage->NotifyUser("Retrieving history to get current step counts", NotifyType::StatusMessage);

    // Use history as an alternative to get the last known step counts.
    create_task(Pedometer::GetSystemHistoryAsync(fromBeginning)).then([this](task<IVectorView<PedometerReading^>^> task)
    {
        try
        {
            auto historyReadings = task.get();
            unsigned int totalStepCount = 0;
            unsigned int historySize = historyReadings->Size;

            if (historySize > 0)
            {
                unsigned int lastIndex = historySize - 1;

                auto timestampFormatter = ref new DateTimeFormatter("day month year hour minute second");
                ScenarioOutput_Timestamp->Text = timestampFormatter->Format(historyReadings->GetAt(lastIndex)->Timestamp);

                // History always returns chronological list of step counts for all PedometerStepKinds
                // And each record represents cumulative step counts for that step kind.
                // So we will use the last set of records - that gives us the cumulative step count for each kind
                for (PedometerStepKind kind = PedometerStepKind::Running; kind >= PedometerStepKind::Unknown; kind--)
                {
                    PedometerReading^ lastReading = historyReadings->GetAt(lastIndex);
                    // decrement index to get to record corresponding to a different stepkind
                    lastIndex--;
                    totalStepCount += lastReading->CumulativeSteps;
                }
            }

            ScenarioOutput_TotalStepCount->Text = totalStepCount.ToString();

            rootPage->NotifyUser("Hit the 'Get steps count' Button", NotifyType::StatusMessage);
        }
        catch (AccessDeniedException^)
        {
            rootPage->NotifyUser("Access to the default pedometer is denied", NotifyType::ErrorMessage);
        }
    });

    // Re-enable the button
    GetCurrentButton->IsEnabled = true;
}
