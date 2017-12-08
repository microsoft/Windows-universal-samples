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
        public const string FEATURE_NAME = "Ink Analysis C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Scenario 1", ClassType=typeof(Scenario1)},
            new Scenario() { Title="Scenario 2", ClassType=typeof(Scenario2)},
        };

        public void NavigateToScenario(Type type)
        {
            for (int index = 0; index < scenarios.Count; index++)
            {
                if (scenarios[index].ClassType == type)
                {
                    ScenarioControl.SelectedIndex = index;
                    break;
                }
            }
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
