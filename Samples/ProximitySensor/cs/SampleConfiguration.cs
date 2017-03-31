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
using ProximityCS;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Proximity sensor";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Data Events", ClassType = typeof(ProximityCS.Scenario1_DataEvents) },
            new Scenario() { Title = "Polling", ClassType = typeof(ProximityCS.Scenario2_Polling) },
            new Scenario() { Title = "Display On/Off", ClassType = typeof(ProximityCS.Scenario3_DisplayOnOff) },
            new Scenario() { Title = "Background Proximity Sensor", ClassType = typeof(ProximityCS.Scenario4_BackgroundProximitySensor) }
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
