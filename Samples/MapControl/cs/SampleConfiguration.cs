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
using Windows.Devices.Geolocation;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "MapControlSample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="MapControl configuration", ClassType=typeof(Scenario1)},
            new Scenario() { Title="Adding MapElements", ClassType=typeof(Scenario2)},
            new Scenario() { Title="Adding XAML children", ClassType=typeof(Scenario3)},
            new Scenario() { Title="Showing 3D locations", ClassType=typeof(Scenario4)},
            new Scenario() { Title="Showing Streetside experience", ClassType=typeof(Scenario5)},
            new Scenario() { Title="Launching Maps using URI Schemes", ClassType=typeof(Scenario6)},
            new Scenario() { Title="Displaying points with clustering", ClassType=typeof(Scenario7)},
            new Scenario() { Title="Find and download Offline Maps", ClassType=typeof(Scenario8)},
            new Scenario() { Title="Custom map appearance", ClassType=typeof(Scenario9)},
        };

        public static readonly Geopoint SeattleGeopoint = new Geopoint(new BasicGeoposition() { Latitude = 47.604, Longitude = -122.329 });
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
