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
using Windows.ApplicationModel.Background;
using Windows.Devices.Sensors;
using Windows.Foundation.Metadata;
using Windows.Phone.UI.Input;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// This page uses methods of the ApiInformation class to detect the presence of
    /// types, methods, events, and enumerated values.
    /// </summary>
    public sealed partial class Scenario1_Basics : Page
    {
        private MainPage rootPage = MainPage.Current;
        private int CameraButtonPressCount = 0;

        // Determining presence requires scanning the system metadata.
        // Instead of repeating the query, calculate the result once and cache it.
        //
        // Does the HardwareButtons class have a CameraPressed event?
        private bool hasCameraButton =
            ApiInformation.IsEventPresent("Windows.Phone.UI.Input.HardwareButtons", "CameraPressed");

        // Does the OrientationSensor class have a GetDefault method that takes two parameters?
        private bool hasOrientationWithOptimizationGoal =
            ApiInformation.IsMethodPresent("Windows.Devices.Sensors.OrientationSensor", "GetDefault", 2);

        // Does the BackgroundAccessStatus enum have an AlwaysAllowed value?
        private bool hasAlwaysAllowedAccess =
            ApiInformation.IsEnumNamedValuePresent("Windows.ApplicationModel.Background.BackgroundAccessStatus", "AlwaysAllowed");

        public Scenario1_Basics()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // If the HardwareButtons.CameraPressed event exists, then register a handler for it.
            if (hasCameraButton)
            {
                HardwareButtons.CameraPressed += HardwareButtons_CameraPressed;
            }
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            // If the HardwareButtons.CameraPressed event exists, then unregister our handler.
            if (hasCameraButton)
            {
                HardwareButtons.CameraPressed -= HardwareButtons_CameraPressed;
            }
        }

        void HardwareButtons_CameraPressed(object sender, CameraEventArgs e)
        {
            CameraButtonPressCount++;
            rootPage.NotifyUser($"Camera button press #{CameraButtonPressCount}", NotifyType.StatusMessage);
        }

        // For more information on using orientation sensors, see the OrientationSensor sample.
        void GetOrientationReport()
        {
            OrientationSensor sensor;
            // If there is a version of GetDefault that lets us specify the optimization goal,
            // then use it. Otherwise, use the regular version.
            if (hasOrientationWithOptimizationGoal)
            {
                sensor = OrientationSensor.GetDefault(SensorReadingType.Absolute, SensorOptimizationGoal.PowerEfficiency);
            }
            else
            {
                sensor = OrientationSensor.GetDefault();
            }

            if (sensor != null)
            {
                OrientationSensorReading reading = sensor.GetCurrentReading();
                SensorQuaternion quaternion = reading.Quaternion;
                rootPage.NotifyUser($"X: {quaternion.X}, Y: {quaternion.Y}, Z: {quaternion.Z}, W: {quaternion.W}", NotifyType.StatusMessage);

                // Restore the default report interval to release resources while the sensor is not in use.
                sensor.ReportInterval = 0;
            }
            else
            {
                rootPage.NotifyUser("Sorry, no sensor found", NotifyType.ErrorMessage);
            }
        }

        // For more information on background activity, see the BackgroundActivity sample.
        async void CheckBackgroundAccessStatus()
        {
            BackgroundAccessStatus status = await BackgroundExecutionManager.RequestAccessAsync();

            // The Windows 10 Anniversary Update introduced a new background access model.
            if (hasAlwaysAllowedAccess)
            {
                switch (status)
                {
                    case BackgroundAccessStatus.AlwaysAllowed:
                    case BackgroundAccessStatus.AllowedSubjectToSystemPolicy:
                        rootPage.NotifyUser("Background activity is allowed.", NotifyType.StatusMessage);
                        break;

                    case BackgroundAccessStatus.Unspecified:
                    case BackgroundAccessStatus.DeniedByUser:
                    case BackgroundAccessStatus.DeniedBySystemPolicy:
                        rootPage.NotifyUser("Background activity is not allowed.", NotifyType.ErrorMessage);
                        break;
                }
            }
            else
            {
                // Use the background access model from Windows 10 November 2015 Update and earlier.
                switch (status)
                {
                    case BackgroundAccessStatus.AllowedMayUseActiveRealTimeConnectivity:
                    case BackgroundAccessStatus.AllowedWithAlwaysOnRealTimeConnectivity:
                        rootPage.NotifyUser("Background activity is allowed.", NotifyType.StatusMessage);
                        break;

                    case BackgroundAccessStatus.Unspecified:
                    case BackgroundAccessStatus.Denied:
                        rootPage.NotifyUser("Background activity is not allowed.", NotifyType.ErrorMessage);
                        break;
                }
            }
        }
    }
}
