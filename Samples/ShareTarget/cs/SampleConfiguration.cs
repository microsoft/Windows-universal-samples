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
        public const string FEATURE_NAME = "Share target C# sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Welcome", ClassType=typeof(Scenario1_Welcome)},
        };
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
        protected override void OnShareTargetActivated(ShareTargetActivatedEventArgs args)
        {
            var rootFrame = CreateRootFrame();
            rootFrame.Navigate(typeof(ShareTarget), args.ShareOperation);
            Window.Current.Activate();
        }
    }

}
