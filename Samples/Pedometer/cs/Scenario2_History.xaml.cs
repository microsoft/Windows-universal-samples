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

using SDKTemplate;

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using Windows.Devices.Enumeration;
using Windows.Devices.Sensors;
using Windows.Globalization;
using Windows.Globalization.DateTimeFormatting;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace PedometerCS
{
    /// <summary>
    /// History Page that gets loaded when 'History' scenario is selected.
    /// </summary>
    public sealed partial class Scenario2_History : Page
    {
        MainPage rootPage = MainPage.Current;

        // Common class ID for pedometers
        Guid PedometerClassId = new Guid("B19F89AF-E3EB-444B-8DEA-202575A71599");

        public Scenario2_History()
        {
            this.InitializeComponent();
            historyRecords = new ObservableCollection<HistoryRecord>();
            historyRecordsList.DataContext = historyRecords;
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
            GetHistory.IsEnabled = false;

            // Determine if we can access pedometers
            var deviceAccessInfo = DeviceAccessInformation.CreateFromDeviceClassId(PedometerClassId);
            if (deviceAccessInfo.CurrentStatus == DeviceAccessStatus.Allowed)
            {
                // Determine if a pedometer is present
                // This can also be done using Windows::Devices::Enumeration::DeviceInformation::FindAllAsync
                var sensor = await Pedometer.GetDefaultAsync();
                if (sensor != null)
                {
                    IReadOnlyList<PedometerReading> historyReadings = null;
                    var timestampFormatter = new DateTimeFormatter("shortdate longtime");

                    // Disable subsequent history retrieval while the async operation is in progress
                    GetHistory.IsEnabled = false;

                    // clear previous content being displayed
                    historyRecords.Clear();

                    try
                    {
                        if (getAllHistory)
                        {
                            var dt = DateTime.FromFileTimeUtc(0);
                            var fromBeginning = new DateTimeOffset(dt);
                            rootPage.NotifyUser("Retrieving all available History", NotifyType.StatusMessage);
                            historyReadings = await Pedometer.GetSystemHistoryAsync(fromBeginning);
                        }
                        else
                        {
                            String notificationString = "Retrieving history from: ";
                            var calendar = new Calendar();
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
                                TimeSpan span = TimeSpan.FromTicks(toTime.Ticks - fromTime.Ticks);
                                rootPage.NotifyUser(notificationString, NotifyType.StatusMessage);
                                historyReadings = await Pedometer.GetSystemHistoryAsync(fromTime, span);
                            }
                        }

                        if (historyReadings != null)
                        {
                            foreach (PedometerReading reading in historyReadings)
                            {
                                HistoryRecord record = new HistoryRecord(reading);
                                historyRecords.Add(record);

                                // Get at most 100 records (number is arbitrary chosen for demonstration purposes)
                                if (historyRecords.Count == 100)
                                {
                                    break;
                                }
                            }

                            rootPage.NotifyUser("History retrieval completed", NotifyType.StatusMessage);
                        }
                    }
                    catch (UnauthorizedAccessException)
                    {
                        rootPage.NotifyUser("User has denied access to activity history", NotifyType.ErrorMessage);
                    }

                    // Finally, re-enable history retrieval
                    GetHistory.IsEnabled = true;
                }
                else
                {
                    rootPage.NotifyUser("No pedometers found", NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("Access to pedometers is denied", NotifyType.ErrorMessage);
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
                        stepString = "Invalid Step Kind";
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
