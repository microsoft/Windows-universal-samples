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
    /// 'Current step count' illustration page
    /// </summary>
    public sealed partial class Scenario3_CurrentStepCount : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario3_CurrentStepCount()
        {
            this.InitializeComponent();
            rootPage.NotifyUser("Hit the 'Get steps count' Button", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Invoked when 'GetCurrentButton' is clicked.
        /// 'ReadingChanged' will not be fired when there is no activity on the pedometer 
        /// and hence can't be reliably used to get the current step count. This handler makes
        /// use of pedometer history on the system to get the current step count of the parameter
        /// </summary>
        async private void GetCurrentButton_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            DateTime dt = DateTime.FromFileTimeUtc(0);
            int totalStepCount = 0;
            int lastTotalCount = 0;

            rootPage.NotifyUser("Retrieving history to get current step counts", NotifyType.StatusMessage);

            // Disable the button while we get the history
            GetCurrentButton.IsEnabled = false;

            DateTimeOffset fromBeginning = new DateTimeOffset(dt);

            IReadOnlyList<PedometerReading> historyReadings = await Pedometer.GetSystemHistoryAsync(fromBeginning);


            // History always returns chronological list of step counts for all PedometerStepKinds
            // And each record represents cumulative step counts for that step kind.
            // So we will use the last set of records - that gives us the cumulative step count for 
            // each kind and ignore rest of the records
            PedometerStepKind stepKind = PedometerStepKind.Unknown;
            DateTimeOffset lastReadingTimestamp;
            bool resetTotal = false;
            foreach (PedometerReading reading in historyReadings)
            {
                if(stepKind == PedometerStepKind.Running)
                {
                    // reset the total after reading the 'PedometerStepKind.Running' count
                    resetTotal = true;
                }

                totalStepCount += reading.CumulativeSteps;
                if (resetTotal)
                {
                    lastReadingTimestamp = reading.Timestamp;
                    lastTotalCount = totalStepCount;
                    stepKind = PedometerStepKind.Unknown;
                    totalStepCount = 0;
                    resetTotal = false;
                }
                else
                {
                    stepKind++;
                }
            }

            ScenarioOutput_TotalStepCount.Text = lastTotalCount.ToString();

            DateTimeFormatter timestampFormatter = new DateTimeFormatter("shortdate longtime");
            ScenarioOutput_Timestamp.Text = timestampFormatter.Format(lastReadingTimestamp);

            rootPage.NotifyUser("Hit the 'Get steps count' Button", NotifyType.StatusMessage);
            GetCurrentButton.IsEnabled = true;
        }
    }
}
