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
using Windows.Devices.Sensors;
using Windows.Foundation;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario2_Polling : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private Barometer sensor;

        public Scenario2_Polling()
        {
            this.InitializeComponent();

            sensor = Barometer.GetDefault();
            if (null == sensor)
            {
                rootPage.NotifyUser("No barometer found", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// This is the click handler for the 'GetData' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ScenarioGetData(object sender, RoutedEventArgs e)
        {
            if (null != sensor)
            {
                BarometerReading reading = sensor.GetCurrentReading();
                if (null != reading)
                {
                    ScenarioOutput_hPa.Text = String.Format("{0,5:0.00}", reading.StationPressureInHectopascals);
                }
            }
            else
            {
                rootPage.NotifyUser("No barometer found", NotifyType.ErrorMessage);
            }
        }
    }
}
