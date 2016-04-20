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
using Windows.Devices.Sensors;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Orientation Sensor C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Choose orientation sensor", ClassType = typeof(Scenario0_Choose) },
            new Scenario() { Title = "Data Events", ClassType = typeof(Scenario1_DataEvents) },
            new Scenario() { Title = "Polling", ClassType = typeof(Scenario2_Polling) },
            new Scenario() { Title = "Calibration", ClassType = typeof(Scenario3_Calibration) }
        };

        public static void SetReadingText(TextBlock textBlock, OrientationSensorReading reading)
        {
            SensorQuaternion quaternion = reading.Quaternion;    // get a reference to the object to avoid re-creating it for each access
            string quaternionReport = string.Format("W: {0,5:0.00}, X: {1,5:0.00}, Y: {2,5:0.00}, Z: {3,5:0.00}",
                quaternion.W, quaternion.X, quaternion.Y, quaternion.Z);

            SensorRotationMatrix rotationMatrix = reading.RotationMatrix;
            string rotationMatrixReport = string.Format(
                "M11: {0,5:0.00}, M12: {1,5:0.00}, M13: {2,5:0.00}\n" +
                "M21: {3,5:0.00}, M22: {4,5:0.00}, M23: {5,5:0.00}\n" +
                "M31: {6,5:0.00}, M32: {7,5:0.00}, M33: {8,5:0.00}",
                rotationMatrix.M11, rotationMatrix.M12, rotationMatrix.M13,
                rotationMatrix.M21, rotationMatrix.M22, rotationMatrix.M23,
                rotationMatrix.M31, rotationMatrix.M32, rotationMatrix.M33);

            string yawAccuracyReport;
            switch (reading.YawAccuracy)
            {
                case MagnetometerAccuracy.Unknown:
                    yawAccuracyReport = "unknown";
                    break;
                case MagnetometerAccuracy.Unreliable:
                    yawAccuracyReport = "unreliable";
                    break;
                case MagnetometerAccuracy.Approximate:
                    yawAccuracyReport = "approximate";
                    break;
                case MagnetometerAccuracy.High:
                    yawAccuracyReport = "high";
                    break;
                default:
                    yawAccuracyReport = "other";
                    break;
            }

            textBlock.Text = "Quaternion:\n" + quaternionReport + "\n\n" +
                "Rotation Matrix:\n" + rotationMatrixReport + "\n\n" +
                "Yaw Accuracy:\n" + yawAccuracyReport;
        }

        public SensorReadingType SensorReadingType { get; set; } = SensorReadingType.Absolute;

        public SensorOptimizationGoal SensorOptimizationGoal { get; set; } = SensorOptimizationGoal.Precision;

        public string SensorDescription
        {
            get
            {
                return SensorReadingType + " orientation sensor with " + SensorOptimizationGoal + " optimization goal";
            }
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
