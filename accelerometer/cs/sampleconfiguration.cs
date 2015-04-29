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
using Windows.UI.Xaml.Controls;
using AccelerometerCS;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "AccelerometerCS";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Data events", ClassType = typeof(AccelerometerCS.Scenario1_DataEvents) },
            new Scenario() { Title = "Shake events", ClassType = typeof(AccelerometerCS.Scenario2_ShakeEvents) },
            new Scenario() { Title = "Polling", ClassType = typeof(AccelerometerCS.Scenario3_Polling) },
            new Scenario() { Title = "OrientationChange", ClassType = typeof(AccelerometerCS.Scenario4_OrientationChanged) },
            new Scenario() { Title = "Data events batching", ClassType = typeof(AccelerometerCS.Scenario5_DataEventsBatching)}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
