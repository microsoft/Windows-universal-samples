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
using ScreenCasting;
using System.Reflection;
using ScreenCasting.Util;

namespace ScreenCasting
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "ScreenCasting";
        private List<Scenario> scenarios;

        private void LoadScenarios()
        {
            if (ApiInformation.IsTypePresent("Windows.Devices.Enumeration.DevicePicker"))
            {
                scenarios = new List<Scenario>
                {
                    new Scenario() { Title="1 - Media Element Casting 101", ClassType=Type.GetType("ScreenCasting.Scenario01",false)},
                    new Scenario() { Title="2 - Casting APIs and a Custom Cast Button", ClassType=Type.GetType("ScreenCasting.Scenario02",false)},
                    new Scenario() { Title="3 - DIAL Sender Windows Universal Application", ClassType=Type.GetType("ScreenCasting.Scenario03",false)},
                    new Scenario() { Title="4 - DIAL Receiver Windows Universal Application", ClassType=Type.GetType("ScreenCasting.Scenario04")},
                    new Scenario() { Title="5 - Multi-View Media Application", ClassType=Type.GetType("ScreenCasting.Scenario05")},
                    new Scenario() { Title="6 - Combine Casting Methods", ClassType=Type.GetType("ScreenCasting.Scenario06")}

              };
            }
            else
            {
                scenarios = new List<Scenario> {
                    new Scenario() { Title="DIAL Receiver Windows Universal Application", ClassType=typeof(Scenario04)}
                };
            }
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}