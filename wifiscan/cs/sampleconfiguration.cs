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
using WiFiScan;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "WiFiScan";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Cached Networks", ClassType=typeof(Scenario1_CachedNetworks)},
            new Scenario() { Title="Scan", ClassType=typeof(Scenario2_Scan)},
            new Scenario() { Title="Register for Updates", ClassType=typeof(Scenario3_RegisterForUpdates)}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
