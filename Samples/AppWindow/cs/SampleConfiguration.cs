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

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "AppWindow C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Create and show secondary window", ClassType=typeof(Scenario1_SecondaryWindow)},
            new Scenario() { Title="Show window on another display", ClassType=typeof(Scenario2_DisplayRegion)},
            new Scenario() { Title="Specific size for secondary window", ClassType=typeof(Scenario3_Size)},
            new Scenario() { Title="CompactOverlay secondary window", ClassType=typeof(Scenario4_CompactOverlay)},
            new Scenario() { Title="Position secondary window", ClassType=typeof(Scenario5_RelativePositioning)},
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
