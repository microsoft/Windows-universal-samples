//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
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

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Geolocation";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Track position", ClassType = typeof(GeolocationCS.Scenario1) },
            new Scenario() { Title = "Get position", ClassType = typeof(GeolocationCS.Scenario2) },
            new Scenario() { Title = "Background position", ClassType = typeof(GeolocationCS.Scenario3) },
            new Scenario() { Title = "Foreground geofencing", ClassType = typeof(GeolocationCS.Scenario4) },
            new Scenario() { Title = "Background geofencing", ClassType = typeof(GeolocationCS.Scenario5) },
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
