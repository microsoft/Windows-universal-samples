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
        public const string FEATURE_NAME = "UserActivity C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Basic User Activity", ClassType=typeof(Scenario1_BasicUserActivity) },
            new Scenario() { Title="Advanced User Activity", ClassType=typeof(Scenario2_AdvancedUserActivity) },
            new Scenario() { Title="Adaptive Cards", ClassType=typeof(Scenario3_AdaptiveCard) },
        };

        public void NavigateToScenarioWithParameter(Scenario scenario, object parameter)
        {
            const String _scenario= "Scenario";
            var i = scenario.ClassType.ToString().IndexOf(_scenario) + _scenario.Length;
            var index = scenario.ClassType.ToString()[i].ToString();
            ScenarioControl.SelectedIndex = int.Parse(index) - 1;
            ScenarioFrame.Navigate(scenario.ClassType, parameter);
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }

    public partial class App : Application
    {
        protected override void OnActivated(IActivatedEventArgs e)
        {
            if (e.Kind == ActivationKind.Protocol)
            {
                Frame rootFrame = CreateRootFrame();

                if (rootFrame.Content == null)
                {
                    if (!rootFrame.Navigate(typeof(MainPage)))
                    {
                        throw new Exception("Failed to create initial page");
                    }
                }

                var mainPage = rootFrame.Content as MainPage;

                // If no matching page found, then just go to the first scenario.
                Scenario destinationScenario = mainPage.Scenarios[0];

                // Look for a matching scenario page.
                var protocolArgs = (IProtocolActivatedEventArgs)e;
                foreach (var scenario in mainPage.Scenarios)
                {
                    var query = (protocolArgs.Uri.Query[0] == '?') ? protocolArgs.Uri.Query.Substring(1) : protocolArgs.Uri.Query;
                    if (scenario.ClassType.Name == query)
                    {
                        destinationScenario = scenario;
                    }
                }

                mainPage.NavigateToScenarioWithParameter(destinationScenario, $"Activated from URI: {protocolArgs.Uri}");

                // Ensure the current window is active
                Window.Current.Activate();
            }
        }
    }
}
