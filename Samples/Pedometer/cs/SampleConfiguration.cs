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
using PedometerCS;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Pedometer";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Events", ClassType=typeof(Scenario1_Events) },
            new Scenario() { Title="History", ClassType=typeof(Scenario2_History) },
            new Scenario() { Title="Current step count", ClassType=typeof(Scenario3_CurrentStepCount) },
            new Scenario() { Title="Background Pedometer", ClassType=typeof(Scenario4_BackgroundPedometer) }
        };
    }
    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
