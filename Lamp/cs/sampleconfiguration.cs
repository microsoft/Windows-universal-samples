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

namespace SDKTemplate
{
    using System;
    using System.Collections.Generic;
    using Windows.UI.Xaml.Controls;
    using Lamp;

    /// <summary>
    /// Main Page Class Object
    /// </summary>
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Lamp";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Get Lamp Instance", ClassType=typeof(Scenario1_GetLamp)},
            new Scenario() { Title="Enable Lamp and Settings Adjustment", ClassType=typeof(Scenario2_EnableSettings)},
            new Scenario() { Title="Lamp Device Change Events", ClassType=typeof(Scenario3_AvailabilityChanged)}
        };
    }

    /// <summary>
    /// Scenario Class Object
    /// </summary>
    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
