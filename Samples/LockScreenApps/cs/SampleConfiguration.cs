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
        public const string FEATURE_NAME = "Lock Screen Apps C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Add app to lock screen", ClassType=typeof(Scenario1_Configure)},
            new Scenario() { Title="Badge and tile notifications", ClassType=typeof(Scenario2_Notifications)},
            new Scenario() { Title="Secondary tiles", ClassType=typeof(Scenario3_SecondaryTiles)},
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
