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
        public const string FEATURE_NAME = "Date and time formatting";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Format date and time using long and short", ClassType = typeof(Scenario1_LongAndShortFormats) },
            new Scenario() { Title = "Format using a string template", ClassType = typeof(Scenario2_StringTemplate) },
            new Scenario() { Title = "Format using a parameterized template", ClassType = typeof(Scenario3_ParameterizedTemplate) },
            new Scenario() { Title = "Override the current user's settings", ClassType = typeof(Scenario4_Override) },
            new Scenario() { Title = "Format using Unicode extensions", ClassType = typeof(Scenario5_UnicodeExtensions) },
            new Scenario() { Title = "Format using different time zones", ClassType = typeof(Scenario6_TimeZone) }
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
