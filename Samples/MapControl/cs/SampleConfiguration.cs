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
using Windows.UI.Xaml.Controls.Maps;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "MapControlSample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="MapControl configuration", ClassType=typeof(Scenario1)},
            new Scenario() { Title="Adding MapElements", ClassType=typeof(Scenario2)},
            new Scenario() { Title="Adding MapLayers", ClassType=typeof(Scenario10)},
            new Scenario() { Title="Adding XAML children", ClassType=typeof(Scenario3)},
            new Scenario() { Title="Showing 3D locations", ClassType=typeof(Scenario4)},
            new Scenario() { Title="Showing Streetside experience", ClassType=typeof(Scenario5)},
            new Scenario() { Title="Launching Maps using URI Schemes", ClassType=typeof(Scenario6)},
            new Scenario() { Title="Displaying points with clustering", ClassType=typeof(Scenario7)},
            new Scenario() { Title="Find and download Offline Maps", ClassType=typeof(Scenario8)},
            new Scenario() { Title="Custom map appearance", ClassType=typeof(Scenario9)},
            new Scenario() { Title="Custom map elements appearance", ClassType=typeof(Scenario11)},
            new Scenario() { Title="Animated MapTileSource", ClassType=typeof(Scenario12)},
        };

        public static readonly Geopoint SeattleGeopoint = new Geopoint(new BasicGeoposition() { Latitude = 47.604, Longitude = -122.329 });

        public static List<MapElement> CreateCityBuildingsCollection()
        {
            return new List<MapElement>
            {
                new MapIcon { Location = new Geopoint(new BasicGeoposition { Latitude = 47.603830, Longitude = -122.329900 }), Title = "Seattle City Hall" },
                new MapIcon { Location = new Geopoint(new BasicGeoposition { Latitude = 47.606709, Longitude = -122.332672 }), Title = "Seattle Public Library" },
                new MapIcon { Location = new Geopoint(new BasicGeoposition { Latitude = 47.604100, Longitude = -122.329204 }), Title = "Municipal Court of Seattle" },
            };
        }

        public static List<MapElement> CreateCityParksCollection()
        {
            return new List<MapElement>
            {
                    new MapIcon { Location = new Geopoint(new BasicGeoposition { Latitude = 47.608313, Longitude = -122.331218 }), Title = "Freeway Park" },
                    new MapIcon { Location = new Geopoint(new BasicGeoposition { Latitude = 47.602029, Longitude = -122.333971 }), Title = "Pioneer Square" },
                    new MapIcon { Location = new Geopoint(new BasicGeoposition { Latitude = 47.601990, Longitude = -122.330681 }), Title = "City Hall Park" },
                    new MapIcon { Location = new Geopoint(new BasicGeoposition { Latitude = 47.600619, Longitude = -122.324957 }), Title = "Kobe Terrace Park" },
            };
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
