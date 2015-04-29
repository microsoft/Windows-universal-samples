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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;

using System;
using System.Threading.Tasks;
using Windows.Devices.Sensors;
using Windows.Foundation;
using Windows.UI.Core;
using Windows.Globalization.DateTimeFormatting;
using System.Collections.ObjectModel;
using System.Collections.Generic;
using Windows.Globalization;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace PedometerCS
{
    /// <summary>
    /// History Page that gets loaded when 'History' scenario is selected.
    /// </summary>
    public sealed partial class Scenario2_History : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario2_History()
        {
            this.InitializeComponent();
            historyRecords = new ObservableCollection<HistoryRecord>();
            historyRecordsList.DataContext = historyRecords;
            rootPage.NotifyUser("Choose time span for history", NotifyType.StatusMessage);
        }

        private void AllHistory_Checked(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            // Collapse SpanPicker - SpanPicker will not be created yet the first time the page is loading
            if (SpanPicker != null)
            {
                SpanPicker.Visibility = Visibility.Collapsed;
            }
            getAllHistory = true;
            rootPage.NotifyUser("This will retrieve all the step count history available on the system", NotifyType.StatusMessage);
        }

        private void SpecificHistory_Checked(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            SpanPicker.Visibility = Visibility.Visible;
            getAllHistory = false;
            rootPage.NotifyUser("This will retrieve all the step count history for the selected time span", NotifyType.StatusMessage);
        }


        /// <summary>
        /// Invoked when 'Get History' button is clicked.
        /// Depending on the user selection, this handler uses one of the overloaded
        /// 'GetSystemHistoryAsync' APIs to retrieve the pedometer history of the user
        /// </summary>
        /// <param name="sender">unused</param>
        /// <param name="e">unused</param>
        async private void GetHistory_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            IReadOnlyList<PedometerReading> historyReadings = null;
            DateTimeFormatter timestampFormatter = new DateTimeFormatter("shortdate longtime");

            // Disable subsequent history retrieval while the async operation is in progress
            GetHistory.IsEnabled = false;

            // clear previous records
            historyRecords.Clear();

            if (getAllHistory)
            {
                DateTime dt = DateTime.FromFileTimeUtc(0);
                DateTimeOffset fromBeginning = new DateTimeOffset(dt);
                rootPage.NotifyUser("Retrieving all available History", NotifyType.StatusMessage);
                historyReadings = await Pedometer.GetSystemHistoryAsync(fromBeginning);
            }
            else
            {
                String notificationString = "Retrieving history from: ";
                Calendar calendar = new Calendar();
                calendar.ChangeClock("24HourClock");

                // DateTime picker will also include hour, minute and seconds from the the system time.
                // Decrement the same to be able to correctly add TimePicker values.

                calendar.SetDateTime(FromDate.Date);
                calendar.AddNanoseconds(-calendar.Nanosecond);
                calendar.AddSeconds(-calendar.Second);
                calendar.AddMinutes(-calendar.Minute);
                calendar.AddHours(-calendar.Hour);
                calendar.AddSeconds(Convert.ToInt32(FromTime.Time.TotalSeconds));

                DateTimeOffset fromTime = calendar.GetDateTime();


                calendar.SetDateTime(ToDate.Date);
                calendar.AddNanoseconds(-calendar.Nanosecond);
                calendar.AddSeconds(-calendar.Second);
                calendar.AddMinutes(-calendar.Minute);
                calendar.AddHours(-calendar.Hour);
                calendar.AddSeconds(Convert.ToInt32(ToTime.Time.TotalSeconds));

                DateTimeOffset toTime = calendar.GetDateTime();

                notificationString += timestampFormatter.Format(fromTime);
                notificationString += " To ";
                notificationString += timestampFormatter.Format(toTime);

                if (toTime.ToFileTime() < fromTime.ToFileTime())
                {
                    rootPage.NotifyUser("Invalid time span. 'To Time' must be equal or more than 'From Time'", NotifyType.ErrorMessage);

                    // Enable subsequent history retrieval while the async operation is in progress
                    GetHistory.IsEnabled = true;
                }
                else
                {
                    TimeSpan span;
                    span = TimeSpan.FromTicks(toTime.Ticks - fromTime.Ticks);
                    rootPage.NotifyUser(notificationString, NotifyType.StatusMessage);
                    historyReadings = await Pedometer.GetSystemHistoryAsync(fromTime, span);
                }
            }

            if (historyReadings != null)
            {
                foreach(PedometerReading reading in historyReadings)
                {
                    HistoryRecord record = new HistoryRecord(reading);
                    historyRecords.Add(record);
                }

                // Finally Enable the 
                GetHistory.IsEnabled = true;
                rootPage.NotifyUser("History retrieval completed", NotifyType.StatusMessage);
            }
        }

        private ObservableCollection<HistoryRecord> historyRecords;
        private bool getAllHistory = true;
    }

    /// <summary>
    /// Represents a history record object that gets binded to the ListView 
    /// controller in the XAML
    /// </summary>
    public sealed class HistoryRecord
    {
        public String TimeStamp
        {
            get
            {
                DateTimeFormatter timestampFormatter = new DateTimeFormatter("shortdate longtime");
                return timestampFormatter.Format(recordTimestamp);
            }
        }

        public String StepKind
        {
            get
            {
                String stepString;
                switch (stepKind)
                {
                    case PedometerStepKind.Unknown:
                        stepString = "Unknown";
                        break;
                    case PedometerStepKind.Walking:
                        stepString = "Walking";
                        break;
                    case PedometerStepKind.Running:
                        stepString = "Running";
                        break;
                    default:
                        stepString = "Invalid Stepy Kind";
                        break;
                }
                return stepString;
            }
        }

        public Int32 StepsCount
        {
            get
            {
                return stepsCount;
            }
        }
        public double StepsDuration
        {
            get
            {
                // return Duration in ms
                return stepsDuration.TotalMilliseconds;
            }
        }

        public HistoryRecord(PedometerReading reading)
        {
            stepKind = reading.StepKind;
            stepsCount = reading.CumulativeSteps;
            recordTimestamp = reading.Timestamp;
        }

        private DateTimeOffset recordTimestamp;
        private PedometerStepKind stepKind;
        private Int32 stepsCount;
        private TimeSpan stepsDuration;
    }
}
