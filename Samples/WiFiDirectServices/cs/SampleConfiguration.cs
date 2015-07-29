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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using SDKTemplate.WiFiDirectServices;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Wi-Fi Direct Services";
        
        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Create Advertiser", ClassType=typeof(Scenario1)},
            new Scenario() { Title="Advertiser Accept", ClassType=typeof(Scenario2)},
            new Scenario() { Title="Discover/Connect Services", ClassType=typeof(Scenario3)},
            new Scenario() { Title="Select Session (Seeker)", ClassType=typeof(Scenario4)},
            new Scenario() { Title="Send Data", ClassType=typeof(Scenario5)}
        };

        public void GoToScenario(int index)
        {
            if (index >= scenarios.Count)
            {
                throw new IndexOutOfRangeException();
            }

            ScenarioControl.SelectedIndex = index;
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
