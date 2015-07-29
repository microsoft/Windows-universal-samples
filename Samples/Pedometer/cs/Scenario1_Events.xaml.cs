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
using Windows.Devices.Sensors;
using Windows.Devices.Enumeration;
using Windows.Foundation;
using Windows.Globalization.DateTimeFormatting;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace PedometerCS
{
    /// <summary>
    /// 'Events' scenario page that gets navigated to when the user selected the 'Events' scenario.
    /// </summary>
    public sealed partial class Scenario1_Events : Page
    {
        // Common class ID for pedometers
        Guid PedometerClassId = new Guid("B19F89AF-E3EB-444B-8DEA-202575A71599");

        MainPage rootPage = MainPage.Current;

        public Scenario1_Events()
        {
            this.InitializeComponent();

            // Register for access changed notifications for pedometers
            deviceAccessInformation = DeviceAccessInformation.CreateFromDeviceClassId(PedometerClassId);
            deviceAccessInformation.AccessChanged += new TypedEventHandler<DeviceAccessInformation, DeviceAccessChangedEventArgs>(AccessChanged);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            base.OnNavigatedFrom(e);
        }

        /// <summary>
        /// 'Register ReadingChanged' button click handler. Registers for the
        /// 'ReadingChanged' event of the default pedometer.
        /// </summary>
        /// <param name="sender">unused</param>
        /// <param name="e">unused</param>
        async private void RegisterButton_Click(object sender, RoutedEventArgs e)
        {
            // Get the default sensor
            if (!registeredForEvents)
            {
                RegisterButton.IsEnabled = false;
                try
                {
                    pedometer = await Pedometer.GetDefaultAsync();
                    if (null == pedometer)
                    {
                        rootPage.NotifyUser("No pedometer available", NotifyType.ErrorMessage);
                    }
                    else
                    {
                        RegisterButton.Content = "Unregister ReadingChanged";
                        RegisterButton.IsEnabled = true;
                        rootPage.NotifyUser("Registered for step count changes", NotifyType.StatusMessage);
                        pedometer.ReportInterval = pedometer.MinimumReportInterval;
                        pedometer.ReadingChanged += Pedometer_ReadingChanged;
                        registeredForEvents = true;
                    }
                }
                catch (System.UnauthorizedAccessException)
                {
                    rootPage.NotifyUser("Access to the default pedometer is denied", NotifyType.ErrorMessage);
                }
                RegisterButton.IsEnabled = true;
            }
            else
            {
                if (pedometer != null)
                {
                    pedometer.ReadingChanged -= Pedometer_ReadingChanged;
                    pedometer = null;
                    rootPage.NotifyUser("Pedometer available - Not registered for events", NotifyType.StatusMessage);
                }
                RegisterButton.Content = "Register ReadingChanged";
                registeredForEvents = false;
            }
        }

        /// <summary>
        /// Invoked when the underlying Pedometer sees a change in the step count for a step kind
        /// </summary>
        /// <param name="sender">unused</param>
        /// <param name="args">Pedometer reading that is being notified</param>
        async private void Pedometer_ReadingChanged(Pedometer sender, PedometerReadingChangedEventArgs args)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                PedometerReading reading = args.Reading;
                int newCount = 0;

                // update step counts based on the step kind
                switch (reading.StepKind)
                {
                    case PedometerStepKind.Unknown:
                        if (reading.CumulativeSteps < unknownStepCount)
                        {
                            unknownStepCount = 0;
                        }
                        newCount = reading.CumulativeSteps - unknownStepCount;
                        unknownStepCount = reading.CumulativeSteps;
                        ScenarioOutput_UnknownCount.Text = unknownStepCount.ToString();
                        ScenarioOutput_UnknownDuration.Text = reading.CumulativeStepsDuration.TotalMilliseconds.ToString();
                        break;
                    case PedometerStepKind.Walking:
                        if (reading.CumulativeSteps < walkingStepCount)
                        {
                            walkingStepCount = 0;
                        }
                        newCount = reading.CumulativeSteps - walkingStepCount;
                        walkingStepCount = reading.CumulativeSteps;
                        ScenarioOutput_WalkingCount.Text = walkingStepCount.ToString();
                        ScenarioOutput_WalkingDuration.Text = reading.CumulativeStepsDuration.TotalMilliseconds.ToString();
                        break;
                    case PedometerStepKind.Running:
                        if (reading.CumulativeSteps < runningStepCount)
                        {
                            runningStepCount = 0;
                        }
                        newCount = reading.CumulativeSteps - runningStepCount;
                        runningStepCount = reading.CumulativeSteps;
                        ScenarioOutput_RunningCount.Text = runningStepCount.ToString();
                        ScenarioOutput_RunningDuration.Text = reading.CumulativeStepsDuration.TotalMilliseconds.ToString();
                        break;
                    default:
                        break;
                }

                totalCumulativeSteps += newCount;
                ScenarioOutput_TotalStepCount.Text = totalCumulativeSteps.ToString();

                DateTimeFormatter timestampFormatter = new DateTimeFormatter("shortdate longtime");
                ScenarioOutput_Timestamp.Text = timestampFormatter.Format(reading.Timestamp);
            });
        }

        /// <summary>
        /// This is the event handler for AccessChanged events.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void AccessChanged(DeviceAccessInformation sender, DeviceAccessChangedEventArgs e)
        {
            var status = e.Status;
            if (status != DeviceAccessStatus.Allowed)
            {
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    rootPage.NotifyUser("Access denied to pedometers", NotifyType.ErrorMessage);
                    pedometer = null;

                    registeredForEvents = false;
                    RegisterButton.Content = "Register ReadingChanged";
                });
            }
        }

        private DeviceAccessInformation deviceAccessInformation = null;
        private Pedometer pedometer = null;
        private bool registeredForEvents = false;
        private Int32 totalCumulativeSteps;
        private Int32 unknownStepCount;
        private Int32 walkingStepCount;
        private Int32 runningStepCount;
    }
}
