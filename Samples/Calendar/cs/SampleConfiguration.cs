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
        public const string FEATURE_NAME = "Calendar C# sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Display a calendar", ClassType = typeof(SDKTemplate.Scenario1_Data) },
            new Scenario() { Title = "Retrieve calendar statistics", ClassType = typeof(SDKTemplate.Scenario2_Stats) },
            new Scenario() { Title = "Calendar enumeration and math", ClassType = typeof(SDKTemplate.Scenario3_Enum) },
            new Scenario() { Title = "Calendar with Unicode extensions in languages", ClassType = typeof(SDKTemplate.Scenario4_UnicodeExtensions) },
            new Scenario() { Title = "Calendar time zone support", ClassType = typeof(SDKTemplate.Scenario5_TimeZone) }
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
