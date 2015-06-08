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
        public const string FEATURE_NAME = "Account picture name C# sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Get display name", ClassType = typeof(GetUserDisplayName) },
            new Scenario() { Title = "Get first and last name", ClassType = typeof(GetUserFirstAndLastName) },
            new Scenario() { Title = "Get account picture", ClassType = typeof(GetAccountPicture) },
            new Scenario() { Title = "Set account picture and listen for changes", ClassType = typeof(SetAccountPicture) }
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
