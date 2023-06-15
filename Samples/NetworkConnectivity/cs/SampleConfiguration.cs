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
        public const string FEATURE_NAME = "NetworkingConnectivity";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Query network connectivity", ClassType = typeof(Scenario1_NetworkConnectivity) },
            new Scenario() { Title = "Get network cost information", ClassType = typeof(Scenario2_NetworkCost) },
            new Scenario() { Title = "Listen to connectivity changes", ClassType = typeof(Scenario3_NetworkConnectivityChanges) },
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
