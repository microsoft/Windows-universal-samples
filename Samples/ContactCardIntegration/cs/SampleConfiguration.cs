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
        public const string FEATURE_NAME = "Contact Card integration C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Create contacts with annotations", ClassType=typeof(Scenario1_CreateContacts)},
            new Scenario() { Title="Receiving a contact", ClassType=typeof(Scenario2_ReceiveContact) },
        };

        public void NavigateToPageWithParameter(int pageIndex, object parameter)
        {
            ScenarioControl.SelectedIndex = pageIndex;
            ScenarioFrame.Navigate(scenarios[pageIndex].ClassType, parameter);
        }

    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }

    public partial class App : Application
    {
        /// <summary>
        // Handle protocol activations.
        /// </summary>
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

                var p = rootFrame.Content as MainPage;
                p.NavigateToPageWithParameter(1, e);

                // Ensure the current window is active
                Window.Current.Activate();
            }
        }
    }
}
