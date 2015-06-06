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

namespace RelativeOrientationCS
{
    public sealed partial class Scenario2_Polling : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage RootPage = MainPage.Current;

        private OrientationSensor Sensor;

        public Scenario2_Polling()
        {
            this.InitializeComponent();

            Sensor = OrientationSensor.GetDefaultForRelativeReadings();
            if (Sensor == null)
            {
                RootPage.NotifyUser("No relative orientation sensor found", NotifyType.ErrorMessage);
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
            OrientationSensorReading reading = Sensor.GetCurrentReading();
            if (reading != null)
            {
                // Quaternion values
                SensorQuaternion quaternion = reading.Quaternion;   // get a reference to the object to avoid re-creating it for each access
                ScenarioOutput_X.Text = String.Format("{0,8:0.00000}", quaternion.X);
                ScenarioOutput_Y.Text = String.Format("{0,8:0.00000}", quaternion.Y);
                ScenarioOutput_Z.Text = String.Format("{0,8:0.00000}", quaternion.Z);
                ScenarioOutput_W.Text = String.Format("{0,8:0.00000}", quaternion.W);

                // Rotation Matrix values
                SensorRotationMatrix rotationMatrix = reading.RotationMatrix;
                ScenarioOutput_M11.Text = String.Format("{0,8:0.00000}", rotationMatrix.M11);
                ScenarioOutput_M12.Text = String.Format("{0,8:0.00000}", rotationMatrix.M12);
                ScenarioOutput_M13.Text = String.Format("{0,8:0.00000}", rotationMatrix.M13);
                ScenarioOutput_M21.Text = String.Format("{0,8:0.00000}", rotationMatrix.M21);
                ScenarioOutput_M22.Text = String.Format("{0,8:0.00000}", rotationMatrix.M22);
                ScenarioOutput_M23.Text = String.Format("{0,8:0.00000}", rotationMatrix.M23);
                ScenarioOutput_M31.Text = String.Format("{0,8:0.00000}", rotationMatrix.M31);
                ScenarioOutput_M32.Text = String.Format("{0,8:0.00000}", rotationMatrix.M32);
                ScenarioOutput_M33.Text = String.Format("{0,8:0.00000}", rotationMatrix.M33);
            }
        }
    }
}
