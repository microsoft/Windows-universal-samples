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

using System;
using System.Threading.Tasks;
using Windows.ApplicationModel.UserDataTasks;
using Windows.Devices.Display.Core;
using Windows.Devices.Power;
using Windows.Globalization.DateTimeFormatting;
using Windows.Globalization.NumberFormatting;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_FindBest : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario1_FindBest()
        {
            InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            PowerGridForecast.ForecastUpdated += PowerGridForecast_ForecastUpdated;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            PowerGridForecast.ForecastUpdated -= PowerGridForecast_ForecastUpdated;
        }

        private void PowerGridForecast_ForecastUpdated(object sender, object e)
        {
            // Notify the user a new forecast is available
            rootPage.NotifyUser("New forecast is available, click \"Find best time\" to find a new best time", NotifyType.StatusMessage);
        }

        // Calculate the index of the forecast entry that contains the requested time.
        // If the time is before the start of the forecast, then returns 0.
        // If the time is past the end of the forecast, then returns the number of forecasts.
        private int GetForecastIndexContainingTime(PowerGridForecast gridForecast, DateTimeOffset time)
        {
            TimeSpan blockDuration = gridForecast.BlockDuration;

            long startBlock = (time - gridForecast.StartTime).Ticks / blockDuration.Ticks;
            return (int)Math.Max(0, Math.Min(startBlock, gridForecast.Forecast.Count));
        }

        private async void FindBest_Click(object sender, RoutedEventArgs e)
        {
            BestTimeRun.Text = "";
            LowestSeverityRun.Text = "";
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            // Collect user inputs.
            if (!uint.TryParse(HoursAheadTextBox.Text, out uint lookAheadHours))
            {
                rootPage.NotifyUser("Unable to parse hours to look ahead", NotifyType.ErrorMessage);
                return;
            }
            TimeSpan lookAhead = TimeSpan.FromHours(lookAheadHours);

            bool restrictToLowUXImpact = LowUXImpactCheckBox.IsChecked.Value;

            // Will be calculated on background thread.
            PowerGridForecast gridForecast = null;
            double lowestSeverity = double.MaxValue;
            DateTimeOffset timeWithLowestSeverity = DateTimeOffset.MaxValue;

            // Do the calculations on a background thread.
            await Task.Run(() =>
            {
                gridForecast = PowerGridForecast.GetForecast();

                // Find the first and last blocks that include the time range we are
                // interested in.
                DateTimeOffset startTime = DateTimeOffset.Now;
                DateTimeOffset endTime = startTime + lookAhead;

                int startBlock = GetForecastIndexContainingTime(gridForecast, startTime);
                int endBlock = GetForecastIndexContainingTime(gridForecast, endTime + gridForecast.BlockDuration);

                for (int index = startBlock; index < endBlock; ++index)
                {
                    PowerGridData data = gridForecast.Forecast[index];

                    // If we are restricting to low impact, then use only low impact time periods.
                    if (restrictToLowUXImpact && !data.IsLowUserExperienceImpact)
                    {
                        continue;
                    }

                    // If the severity is not an improvement, then don't use this one.
                    if (data.Severity >= lowestSeverity)
                    {
                        continue;
                    }

                    lowestSeverity = data.Severity;
                    timeWithLowestSeverity = gridForecast.StartTime + new TimeSpan(index * gridForecast.BlockDuration.Ticks);
                }
            });

            // Report the results.
            if (lowestSeverity <= 1.0)
            {
                var dateFormatter = new DateTimeFormatter("shortdate shorttime");
                var severityFormatter = new DecimalFormatter()
                {
                    FractionDigits = 2,
                    IntegerDigits = 1,
                    IsDecimalPointAlwaysDisplayed = true,
                    NumberRounder = new IncrementNumberRounder() { Increment = 0.01 }
                };
                BestTimeRun.Text = dateFormatter.Format(timeWithLowestSeverity) + " to " + dateFormatter.Format(timeWithLowestSeverity + gridForecast.BlockDuration);
                LowestSeverityRun.Text = severityFormatter.Format(lowestSeverity);
            }
            else
            {
                rootPage.NotifyUser("Unable to find a good time to do work", NotifyType.ErrorMessage);
            }
        }
    }
}
