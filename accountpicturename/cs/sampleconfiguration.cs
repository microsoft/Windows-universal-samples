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
using Windows.ApplicationModel;
using Windows.ApplicationModel.Activation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

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

    /// <summary>
    /// Provides sample-specific behavior to supplement the default Application class.
    /// </summary>
    sealed partial class App : Application
    {
        protected override void OnActivated(IActivatedEventArgs args)
        {
            // Check to see if the app was activated via a protocol
            if (args.Kind == ActivationKind.Protocol)
            {
                var protocolArgs = (ProtocolActivatedEventArgs)args;

                // This app was activated via the Account picture apps section in PC Settings / Personalize / Account picture.
                // Here you would do app-specific logic so that the user receives account picture selection UX.
                if (protocolArgs.Uri.Scheme == "ms-accountpictureprovider")
                {
                    // The Content might be null if App has not yet been activated, if so first activate the main page.
                    if (Window.Current.Content == null)
                    {
                        Frame rootFrame = new Frame();
                        rootFrame.Navigate(typeof(MainPage));
                        Window.Current.Content = rootFrame;
                        Window.Current.Activate();
                    }
                    MainPage.Current.NavigateToScenario(typeof(SDKTemplate.SetAccountPicture));
                }
            }
        }
    }
}
