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
    public partial class App
    {
        /// <summary>
        /// Invoked when the application is launched by a protocol.
        /// </summary>
        /// <param name="args">Container for the activation information including the Uri used to activate this app.</param>
        protected override void OnActivated(IActivatedEventArgs args)
        {
            Frame rootFrame = Window.Current.Content as Frame;
            if (rootFrame == null)
            {
                rootFrame = new Frame();
                rootFrame.Navigate(typeof(MainPage));
                Window.Current.Content = rootFrame;
            }

            // Protocol activation is the only type of activation that this app handles
            if (args.Kind == ActivationKind.Protocol)
            {
                ProtocolActivatedEventArgs protocolArgs = args as ProtocolActivatedEventArgs;
                ((MainPage)rootFrame.Content).LoadScenarioForProtocolActivation(protocolArgs);
            }

            Window.Current.Activate();
        }

    }
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Share source C# sample";
        public const string MissingTitleError = "Enter a title for what you are sharing and try again.";

        public List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Share text",                  ClassType = typeof(ShareText) },
            new Scenario() { Title = "Share web link",              ClassType = typeof(ShareWebLink) },
            new Scenario() { Title = "Share application link",      ClassType = typeof(ShareApplicationLink) },
            new Scenario() { Title = "Share image",                 ClassType = typeof(ShareImage) },
            new Scenario() { Title = "Share files",                 ClassType = typeof(ShareFiles) },
            new Scenario() { Title = "Share delay rendered files",  ClassType = typeof(ShareDelayRenderedFiles) },
            new Scenario() { Title = "Share HTML content",          ClassType = typeof(ShareHtml) },
            new Scenario() { Title = "Share custom data",           ClassType = typeof(ShareCustomData) },
            new Scenario() { Title = "Fail with display text",      ClassType = typeof(SetErrorMessage) }
        };

        /// <summary>
        /// Selects the scenario corresponding with the Uri used to activate this app via a protocol. If there is no corresponding
        /// scenario then there is no effect.
        /// </summary>
        /// <param name="protocolArgs">Container for the protocol activation information which includes the Uri that activated this app</param>
        public void LoadScenarioForProtocolActivation(ProtocolActivatedEventArgs protocolArgs)
        {
            foreach (Scenario scenario in scenarios)
            {
                if (protocolArgs.Uri.Equals(SharePage.GetApplicationLink(scenario.ClassType.Name)))
                {
                    // By setting the selected index, the Scenarios_SelectionChanged(...) method will be called
                    ScenarioControl.SelectedIndex = scenarios.IndexOf(scenario);
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
