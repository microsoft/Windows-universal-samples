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
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Devices.Power;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Globalization.DateTimeFormatting;
using System.Linq;
using Windows.UI.Xaml.Data;
using Windows.Globalization.NumberFormatting;

namespace SDKTemplate
{
    public class ForecastItem
    {
        public string DateTimeString;
        public string SeverityString;
        public string LowImpactString;
    }

    public sealed partial class Scenario2_PrintFullForecast : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario2_PrintFullForecast()
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
            rootPage.NotifyUser("New forecast is available, click \"Get forecast\" to show the new forecast", NotifyType.StatusMessage);
        }

        private async void GetForecastButton_Click(object sender, RoutedEventArgs e)
        {
            // Erase the previous forecast.
            ForecastStartTimeRun.Text = "";
            ForecastBlockDurationRun.Text = "";
            ForecastList.ItemsSource = null;

            // Use a background thread to get the forecast, so we don't block the UI thread.
            PowerGridForecast gridForecast = await Task.Run(() => PowerGridForecast.GetForecast());

            // If the API cannot obtain a forecast, the forecast is empty
            if (gridForecast.Forecast.Count == 0)
            {
                rootPage.NotifyUser("No forecast available. Try again later.", NotifyType.ErrorMessage);
                return;
            }

            // Add the forecast to the output.
            DateTimeOffset blockStartTime = gridForecast.StartTime;
            TimeSpan blockDuration = gridForecast.BlockDuration;

            // Set the UI for the start time and duration
            ForecastStartTimeRun.Text = blockStartTime.ToString("F");
            ForecastBlockDurationRun.Text = blockDuration.TotalMinutes.ToString();

            // Start with a header item.
            var items = new List<ForecastItem>
            {
                new ForecastItem { DateTimeString = "Date/Time", SeverityString = "Severity", LowImpactString = "Low impact?" }
            };
            var dateFormatter = new DateTimeFormatter("shortdate shorttime");
            var severityFormatter = new DecimalFormatter()
            {
                FractionDigits = 2,
                IntegerDigits = 1,
                IsDecimalPointAlwaysDisplayed = true,
                NumberRounder = new IncrementNumberRounder() { Increment = 0.01 }
            };

            // Add each block as an item in the forecast list.
            foreach (PowerGridData data in gridForecast.Forecast)
            {
                items.Add(new ForecastItem
                {
                    DateTimeString = dateFormatter.Format(blockStartTime),
                    SeverityString = severityFormatter.Format(data.Severity),
                    LowImpactString = data.IsLowUserExperienceImpact.ToString()
                });
                blockStartTime += blockDuration;
            }

            ForecastList.ItemsSource = items;
        }
    }
}
