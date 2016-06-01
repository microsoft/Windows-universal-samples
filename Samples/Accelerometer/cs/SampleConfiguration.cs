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
        public const string FEATURE_NAME = "Accelerometer C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Choose accelerometer", ClassType = typeof(Scenario0_Choose) },
            new Scenario() { Title = "Data events", ClassType = typeof(Scenario1_DataEvents) },
            new Scenario() { Title = "Shake events", ClassType = typeof(Scenario2_ShakeEvents) },
            new Scenario() { Title = "Polling", ClassType = typeof(Scenario3_Polling) },
            new Scenario() { Title = "Orientation change", ClassType = typeof(Scenario4_OrientationChanged) },
            new Scenario() { Title = "Data events batching", ClassType = typeof(Scenario5_DataEventsBatching)}
        };

        public static void SetReadingText(TextBlock textBlock, AccelerometerReading reading)
        {
            textBlock.Text = string.Format("X: {0,5:0.00}, Y: {1,5:0.00}, Z: {2,5:0.00}",
                reading.AccelerationX, reading.AccelerationY, reading.AccelerationZ);
        }

        public AccelerometerReadingType AccelerometerReadingType { get; set; } = AccelerometerReadingType.Standard;
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
