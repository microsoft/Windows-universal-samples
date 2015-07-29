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
using GyrometerCS;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Gyrometer";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Data Events", ClassType = typeof(GyrometerCS.Scenario1_DataEvents) },
            new Scenario() { Title = "Polling", ClassType = typeof(GyrometerCS.Scenario2_Polling) },
            new Scenario() { Title = "Cross-Platform Porting", ClassType = typeof(GyrometerCS.Scenario3_Porting) },
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
