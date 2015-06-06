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
using Windows.Devices.Sensors;
using Windows.Foundation;
using System.Threading.Tasks;
using Windows.UI.Core;

namespace SimpleOrientationCS
{
    public sealed partial class Scenario2_Polling : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private SimpleOrientationSensor _sensor;

        public Scenario2_Polling()
        {
            this.InitializeComponent();

            _sensor = SimpleOrientationSensor.GetDefault();
            if (_sensor == null)
            {
                rootPage.NotifyUser("No simple orientation sensor found", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Helper method to display the device orientation in the specified text box.
        /// </summary>
        /// <param name="tb">
        /// The text box receiving the orientation value.
        /// </param>
        /// <param name="orientation">
        /// The orientation value.
        /// </param>
        private void DisplayOrientation(TextBlock tb, SimpleOrientation orientation)
        {
            switch (orientation)
            {
                case SimpleOrientation.NotRotated:
                    tb.Text = "Not Rotated";
                    break;
                case SimpleOrientation.Rotated90DegreesCounterclockwise:
                    tb.Text = "Rotated 90 Degrees Counterclockwise";
                    break;
                case SimpleOrientation.Rotated180DegreesCounterclockwise:
                    tb.Text = "Rotated 180 Degrees Counterclockwise";
                    break;
                case SimpleOrientation.Rotated270DegreesCounterclockwise:
                    tb.Text = "Rotated 270 Degrees Counterclockwise";
                    break;
                case SimpleOrientation.Faceup:
                    tb.Text = "Faceup";
                    break;
                case SimpleOrientation.Facedown:
                    tb.Text = "Facedown";
                    break;
                default:
                    tb.Text = "Unknown orientation";
                    break;
            }
        }

        /// <summary>
        /// This is the click handler for the 'Enable' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ScenarioGet(object sender, RoutedEventArgs e)
        {
            if (_sensor != null)
            {
                DisplayOrientation(ScenarioOutput_Orientation, _sensor.GetCurrentOrientation());
            }
            else
            {
                rootPage.NotifyUser("No simple orientation sensor found", NotifyType.ErrorMessage);
            }
        }
    }
}
