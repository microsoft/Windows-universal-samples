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
#include "Scenario2_History.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario2_History::Scenario2_History() : rootPage(MainPage::Current)
{
    InitializeComponent();

    // Disable buttons until we can access a pedometer
    GetHistory->IsEnabled = false;

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
                historyRecords = ref new Vector<HistoryRecord^>();
                historyRecordsList->DataContext = historyRecords;
                rootPage->NotifyUser("Choose time span for history", NotifyType::StatusMessage);
                GetHistory->IsEnabled = true;
            }
        });
    }
    else
    {
        rootPage->NotifyUser("Access to pedometer is denied", NotifyType::ErrorMessage);
    }
}

/// <summary>
/// Invoked when history span radio buttons are checked. This one handles the 
/// case when user selects 'All of the History' radio button.
/// we collapse the date and time pickers here and is used to make a note of the 
/// user selection
/// </summary>
/// <param name="sender">unused</param>
/// <param name="e">unused</param>
void Scenario2_History::AllHistory_Checked(Platform::Object^ /*sender*/, Windows::UI::Xaml::RoutedEventArgs^ /*e*/)
{
    // Collapse SpanPicker - SpanPicker will not be created yet the first time the page is loading
    if (nullptr != SpanPicker)
    {
        SpanPicker->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    }
    getAllHistory = true;
    rootPage->NotifyUser("This will retrieve all the step count history available on the system", NotifyType::StatusMessage);
}

/// <summary>
/// Invoked when history span radio buttons are checked. This one handles the 
/// case when user selects 'History for given time span' radio button.
/// we make the date and time pickers visible here for user selection.
/// </summary>
/// <param name="sender">unused</param>
/// <param name="e">unused</param>
void Scenario2_History::SpecificHistory_Checked(Platform::Object^ /*sender*/, Windows::UI::Xaml::RoutedEventArgs^ /*e*/)
{
    SpanPicker->Visibility = Windows::UI::Xaml::Visibility::Visible;
    getAllHistory = false;
    rootPage->NotifyUser("This will retrieve all the step count history for the selected time span", NotifyType::StatusMessage);
}

/// <summary>
/// Invoked when 'Get History' button is clicked.
/// Depending on the user selection, this handler uses one of the overloaded
/// 'GetSystemHistoryAsync' APIs to retrieve the pedometer history of the user
/// </summary>
/// <param name="sender">unused</param>
/// <param name="e">unused</param>
void Scenario2_History::GetHistory_Click(Platform::Object^ /*sender*/, Windows::UI::Xaml::RoutedEventArgs^ /*e*/)
{
    IAsyncOperation<IVectorView<PedometerReading^>^>^ getHistoryAsyncOp = nullptr;
    String^ notificationString = ref new String();
    DateTimeFormatter^ timestampFormatter = ref new DateTimeFormatter("shortdate longtime");

    // clear previous content being displayed
    historyRecords->Clear();

    // Disable subsequent history retrieval while the async operation is in progress
    GetHistory->IsEnabled = false;

    if (getAllHistory)
    {
        DateTime fromBeginning = {};
        getHistoryAsyncOp = Pedometer::GetSystemHistoryAsync(fromBeginning);
        notificationString += L" Retrieving all available History";
    }
    else
    {
        // Calculate the timepsan from the user selection of the history span.
        // you may skip this part to follow the 'GetSystemHistoryAsync' API usage
        notificationString += L"Retrieving history from: ";
        Calendar^ calendar = ref new Calendar();
        calendar->ChangeClock("24HourClock");

        // DateTime picker will also include hour, minute and seconds from the the system time.
        // Decrement the same to be able to correctly add TimePicker values.
        calendar->SetDateTime(FromDate->Date);
        calendar->AddNanoseconds(-calendar->Nanosecond);
        calendar->AddSeconds(-calendar->Second);
        calendar->AddMinutes(-calendar->Minute);
        calendar->AddHours(-calendar->Hour);

        DateTime fromTime = {};
        fromTime.UniversalTime = (calendar->GetDateTime()).UniversalTime + FromTime->Time.Duration;

        calendar->SetDateTime(ToDate->Date);
        calendar->AddNanoseconds(-calendar->Nanosecond);
        calendar->AddSeconds(-calendar->Second);
        calendar->AddMinutes(-calendar->Minute);
        calendar->AddHours(-calendar->Hour);

        DateTime toTime = {};
        toTime.UniversalTime = (calendar->GetDateTime()).UniversalTime + ToTime->Time.Duration;

        notificationString += timestampFormatter->Format(fromTime);
        notificationString += L" To ";
        notificationString += timestampFormatter->Format(toTime);

        if (toTime.UniversalTime < fromTime.UniversalTime)
        {
            rootPage->NotifyUser("Invalid time span. 'To Time' must be equal or more than 'From Time'", NotifyType::ErrorMessage);
            // Enable subsequent history retrieval while the async operation is in progress
            GetHistory->IsEnabled = true;
        }
        else
        {
            TimeSpan span;
            span.Duration = toTime.UniversalTime - fromTime.UniversalTime;
            getHistoryAsyncOp = Pedometer::GetSystemHistoryAsync(fromTime, span);
        }
    }

    if (getHistoryAsyncOp != nullptr)
    {
        rootPage->NotifyUser(notificationString, NotifyType::StatusMessage);
        create_task(getHistoryAsyncOp).then([this](task<IVectorView<PedometerReading^>^> task)
        {
            try
            {
                auto historyReadings = task.get();

                if (historyReadings != nullptr)
                {
                    // Get at most 100 records (number is arbitrary chosen for demonstration purposes)
                    unsigned int count = min(100, historyReadings->Size);

                    for (unsigned int index = 0; index < count; index++)
                    {
                        HistoryRecord^ record = ref new HistoryRecord(historyReadings->GetAt(index));
                        historyRecords->Append(record);
                    }
                }

                rootPage->NotifyUser("History retrieval completed", NotifyType::StatusMessage);
            }
            catch (AccessDeniedException^)
            {
                rootPage->NotifyUser("Access to the default pedometer is denied", NotifyType::ErrorMessage);
            }
        });
    }

    // Finally, re-enable history retrieval
    GetHistory->IsEnabled = true;
}
