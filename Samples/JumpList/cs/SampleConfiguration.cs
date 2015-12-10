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
using Windows.ApplicationModel.Activation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Jump List C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Respond to being launched", ClassType=typeof(Scenario1_Launched)},
            new Scenario() { Title="Remove the jump list", ClassType=typeof(Scenario2_Remove)},
            new Scenario() { Title="Creating custom items", ClassType=typeof(Scenario3_CustomItems)},
            new Scenario() { Title="Change the system group", ClassType=typeof(Scenario4_ChangeSystemGroup)},
            new Scenario() { Title="Check if the platform is supported", ClassType=typeof(Scenario5_IsSupported)},
        };

        private string launchParam;
        public string LaunchParam
        {
            get
            {
                return launchParam;
            }

            set
            {
                launchParam = value;
                if (!string.IsNullOrEmpty(value))
                {
                    if (ScenarioControl.SelectedIndex == 0)
                    {
                        ScenarioControl.SelectedIndex = -1;
                    }

                    ScenarioControl.SelectedIndex = 0;
                }
            }
        }
    }

    public partial class App : Application
    {
        protected override void OnFileActivated(FileActivatedEventArgs args)
        {
            base.OnFileActivated(args);

            if (args.Files.Count == 1)
            {
                MainPage.Current.LaunchParam = "file: " + args.Files[0].Name;
            }
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
