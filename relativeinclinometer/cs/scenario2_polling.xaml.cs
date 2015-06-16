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

using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using Windows.Devices.Sensors;

namespace RelativeInclinometerCS
{
    public sealed partial class Scenario2_Polling : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage RootPage = MainPage.Current;

        private Inclinometer Sensor;

        public Scenario2_Polling()
        {
            this.InitializeComponent();

            Sensor = Inclinometer.GetDefaultForRelativeReadings();
            if (Sensor == null)
            {
                RootPage.NotifyUser("No relative inclinometer found", NotifyType.ErrorMessage);
                GetDataButton.IsEnabled = false;
            }
            else
            {
                GetDataButton.IsEnabled = true;
            }
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached. The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
        }

        /// <summary>
        /// Invoked immediately before the Page is unloaded and is no longer the current source of a parent Frame.
        /// </summary>
        /// <param name="e">
        /// Event data that can be examined by overriding code. The event data is representative
        /// of the navigation that will unload the current Page unless canceled. The
        /// navigation can potentially be canceled by setting Cancel.
        /// </param>
        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            base.OnNavigatingFrom(e);
        }

        /// <summary>
        /// Invoked when a user clicks on the GetDataButton
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private void GetData(object sender, object args)
        {
            InclinometerReading reading = Sensor.GetCurrentReading();
            if (reading != null)
            {
                ScenarioOutput_X.Text = String.Format("{0,5:0.00}", reading.PitchDegrees);
                ScenarioOutput_Y.Text = String.Format("{0,5:0.00}", reading.RollDegrees);
                ScenarioOutput_Z.Text = String.Format("{0,5:0.00}", reading.YawDegrees);
            }
        }
    }
}
