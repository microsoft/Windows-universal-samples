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

namespace PedometerCS
{
    /// <summary>
    /// 'Events' scenario page that gets navigated to when the user selected the 'Events' scenario.
    /// </summary>
    public sealed partial class Scenario1_Events : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario1_Events()
        {
            this.InitializeComponent();
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
        /// 'Register ReadingChanged' button click handler. Registers to the '
        /// ReadingChanged' event of the default pedometer opened earlier.
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
                    if(pedometer == null)
                    {
                        rootPage.NotifyUser("No pedometer available", NotifyType.ErrorMessage);
                    }
                    RegisterButton.Content = "UnRegister ReadingChanged";
                    RegisterButton.IsEnabled = true;
                    rootPage.NotifyUser("Registered for step count changes", NotifyType.StatusMessage);
                }
                catch (System.UnauthorizedAccessException)
                {
                    rootPage.NotifyUser("Access to the default pedometer is denied", NotifyType.ErrorMessage);
                }
                pedometer.ReadingChanged += Pedometer_ReadingChanged;
                registeredForEvents = true;
            }
            else
            {
                pedometer.ReadingChanged -= Pedometer_ReadingChanged;
                pedometer = null;
                RegisterButton.Content = "Register ReadingChanged";
                registeredForEvents = false;
                rootPage.NotifyUser("Pedometer available - Not registered for events", NotifyType.StatusMessage);
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
                        break;
                    case PedometerStepKind.Walking:
                        if (reading.CumulativeSteps < walkingStepCount)
                        {
                            walkingStepCount = 0;
                        }
                        newCount = reading.CumulativeSteps - walkingStepCount;
                        walkingStepCount = reading.CumulativeSteps;
                        ScenarioOutput_WalkingCount.Text = walkingStepCount.ToString();
                        break;
                    case PedometerStepKind.Running:
                        if (reading.CumulativeSteps < runningStepCount)
                        {
                            runningStepCount = 0;
                        }
                        newCount = reading.CumulativeSteps - runningStepCount;
                        runningStepCount = reading.CumulativeSteps;
                        ScenarioOutput_RunningCount.Text = runningStepCount.ToString();
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

        private Pedometer pedometer = null;
        private bool registeredForEvents = false;
        private Int32 totalCumulativeSteps;
        private Int32 unknownStepCount;
        private Int32 walkingStepCount;
        private Int32 runningStepCount;

    }
}
