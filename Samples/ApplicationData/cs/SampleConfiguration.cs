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

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Files", ClassType = typeof(Scenario1_Files) },
            new Scenario() { Title = "Settings", ClassType = typeof(Scenario2_Settings) },
            new Scenario() { Title = "Setting Containers", ClassType = typeof(Scenario3_SettingContainer) },
            new Scenario() { Title = "Composite Settings", ClassType = typeof(Scenario4_CompositeSettings) },
            new Scenario() { Title = "ms-appdata:// Protocol", ClassType = typeof(Scenario5_Msappdata) },
            new Scenario() { Title = "Clear", ClassType = typeof(Scenario6_ClearScenario) },
            new Scenario() { Title = "SetVersion", ClassType = typeof(Scenario7_SetVersion) },
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
