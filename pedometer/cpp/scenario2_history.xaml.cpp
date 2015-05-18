// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario2_History.xaml.h"

using namespace PedometerCPP;
using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Devices::Sensors;
using namespace Concurrency;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::Globalization;
using namespace Windows::UI::Core;
using namespace Platform::Collections;

Scenario2_History::Scenario2_History() : rootPage(MainPage::Current)
{
    InitializeComponent();
    historyRecords = ref new Vector<HistoryRecord^>();
    historyRecordsList->DataContext = historyRecords;
    rootPage->NotifyUser("Choose time span for history", NotifyType::StatusMessage);
}

/// <summary>
/// Invoked when history span radio buttons are checked. This one handles the 
/// case when user selects 'All of the History' radio button.
/// we collapse the date and time pickers here and is used to make a note of the 
/// user selection
/// </summary>
/// <param name="sender">unused</param>
/// <param name="e">unused</param>
void PedometerCPP::Scenario2_History::AllHistory_Checked(Platform::Object^ /*sender*/, Windows::UI::Xaml::RoutedEventArgs^ /*e*/)
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
void PedometerCPP::Scenario2_History::SpecificHistory_Checked(Platform::Object^ /*sender*/, Windows::UI::Xaml::RoutedEventArgs^ /*e*/)
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
void PedometerCPP::Scenario2_History::GetHistory_Click(Platform::Object^ /*sender*/, Windows::UI::Xaml::RoutedEventArgs^ /*e*/)
{
    IAsyncOperation<IVectorView<PedometerReading^>^>^ getHistoryAsync;
    bool waitforHistory = true;
    String^ notificationString = ref new String();
    DateTimeFormatter^ timestampFormatter = ref new DateTimeFormatter("shortdate longtime");

    // Disable subsequent history retrieval while the async operation is in progress
    GetHistory->IsEnabled = false;

    // clear previous records
    historyRecords->Clear();

    if (getAllHistory)
    {
        DateTime fromBeginning = {};
        getHistoryAsync = Pedometer::GetSystemHistoryAsync(fromBeginning);
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
            waitforHistory = false;
        }
        else
        {
            TimeSpan span;
            span.Duration = toTime.UniversalTime - fromTime.UniversalTime;
            getHistoryAsync = Pedometer::GetSystemHistoryAsync(fromTime, span);
        }
    }

    if (waitforHistory)
    {
        auto historyTask = create_task(getHistoryAsync);
        // rootPage->NotifyUser("Waiting for Async History Retrieval", NotifyType::StatusMessage);
        rootPage->NotifyUser(notificationString, NotifyType::StatusMessage);
        historyTask.then([this](IVectorView<PedometerReading^>^ historyReadings)
        {
            for (unsigned int index = 0; index < historyReadings->Size; index++)
            {
                HistoryRecord^ record = ref new HistoryRecord(historyReadings->GetAt(index));
                historyRecords->Append(record);
            }

            // Finally Enable the 
            GetHistory->IsEnabled = true;
            rootPage->NotifyUser("History retrieval completed", NotifyType::StatusMessage);
        });
    }
}
