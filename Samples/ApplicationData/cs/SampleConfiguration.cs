//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
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
        public const string FEATURE_NAME = "ApplicationData";

        // Change the array below to reflect the name of your scenarios.
        // This will be used to populate the list of scenarios on the main page with
        // which the user will choose the specific scenario that they are interested in.
        // These should be in the form: "Navigating to a web page".
        // The code in MainPage will take care of turning this into: "1) Navigating to a web page"
        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Files", ClassType = typeof(ApplicationDataSample.Files) },
            new Scenario() { Title = "Settings", ClassType = typeof(ApplicationDataSample.Settings) },
            new Scenario() { Title = "Setting Containers", ClassType = typeof(ApplicationDataSample.SettingContainer) },
            new Scenario() { Title = "Composite Settings", ClassType = typeof(ApplicationDataSample.CompositeSettings) },
            new Scenario() { Title = "DataChanged Event", ClassType = typeof(ApplicationDataSample.DataChangedEvent) },
            new Scenario() { Title = "Roaming: HighPriority", ClassType = typeof(ApplicationDataSample.HighPriority) },
            new Scenario() { Title = "ms-appdata:// Protocol", ClassType = typeof(ApplicationDataSample.Msappdata) },
            new Scenario() { Title = "Clear", ClassType = typeof(ApplicationDataSample.ClearScenario) },
            new Scenario() { Title = "SetVersion", ClassType = typeof(ApplicationDataSample.SetVersion) },
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }

        public override string ToString()
        {
            return Title;
        }
    }
}
