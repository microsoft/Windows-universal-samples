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
using MapControlSample;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "MapControlSample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="MapControl Configuration", ClassType=typeof(Scenario1)},
            new Scenario() { Title="Adding MapElements to MapControl", ClassType=typeof(Scenario2)},
            new Scenario() { Title="Adding XAML children to MapControl", ClassType=typeof(Scenario3)},
            new Scenario() { Title="Showing 3D locations in the MapControl", ClassType=typeof(Scenario4)},
            new Scenario() { Title="Showing Streetside experience within the MapControl", ClassType=typeof(Scenario5)},
            new Scenario() { Title= "Launching Maps using URI Schemes", ClassType=typeof(Scenario6)}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
