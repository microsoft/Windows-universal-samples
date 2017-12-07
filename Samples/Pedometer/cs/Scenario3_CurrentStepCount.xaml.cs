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
using Windows.Devices.Enumeration;
using Windows.Devices.Sensors;
using Windows.Globalization.DateTimeFormatting;
using Windows.UI.Xaml.Controls;

namespace PedometerCS
{
    /// <summary>
    /// 'Current step count' illustration page
    /// </summary>
    public sealed partial class Scenario3_CurrentStepCount : Page
    {
        MainPage rootPage = MainPage.Current;

        // Common class ID for pedometers
        Guid PedometerClassId = new Guid("B19F89AF-E3EB-444B-8DEA-202575A71599");

        public Scenario3_CurrentStepCount()
        {
            this.InitializeComponent();
            rootPage.NotifyUser("Hit the 'Get steps count' Button", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Invoked when 'GetCurrentButton' is clicked.
        /// 'ReadingChanged' will not be fired when there is no activity on the pedometer 
        /// and hence can't be reliably used to get the current step count. This handler makes
        /// use of 'GetCurrentReadings' API to determine the current step count
        /// </summary>
        async private void GetCurrentButton_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            // Determine if we can access pedometers
            var deviceAccessInfo = DeviceAccessInformation.CreateFromDeviceClassId(PedometerClassId);
            if (deviceAccessInfo.CurrentStatus == DeviceAccessStatus.Allowed)
            {
                // Determine if a pedometer is present
                // This can also be done using Windows::Devices::Enumeration::DeviceInformation::FindAllAsync
                var sensor = await Pedometer.GetDefaultAsync();
                if (sensor != null)
                {
                    DateTime dt = DateTime.FromFileTimeUtc(0);
                    int totalStepCount = 0;

                    // Get the current readings to find the current step counters
                    var currentReadings = sensor.GetCurrentReadings();
                    bool updateTimestamp = true;
                    foreach (PedometerStepKind kind in Enum.GetValues(typeof(PedometerStepKind)))
                    {
                        PedometerReading reading;
                        if (currentReadings.TryGetValue(kind, out reading))
                        {
                            totalStepCount += reading.CumulativeSteps;
                        }
                        if (updateTimestamp)
                        {
                            DateTimeFormatter timestampFormatter = new DateTimeFormatter("shortdate longtime");
                            ScenarioOutput_Timestamp.Text = timestampFormatter.Format(reading.Timestamp);
                            updateTimestamp = false;
                        }
                    }

                    ScenarioOutput_TotalStepCount.Text = totalStepCount.ToString();
                    rootPage.NotifyUser("Hit the 'Get steps count' Button", NotifyType.StatusMessage);

                    // Re-enable button
                    GetCurrentButton.IsEnabled = true;
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
    }
}
