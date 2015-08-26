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
        public const string FEATURE_NAME = "Globalization preferences C# sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="User preferences", ClassType=typeof(Scenario1_Prefs) },
            new Scenario() { Title="Language characteristics", ClassType=typeof(Scenario2_Lang) },
            new Scenario() { Title="Region characteristics", ClassType=typeof(Scenario3_Region) },
            new Scenario() { Title="Current input language", ClassType=typeof(Scenario4_Input) },
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
