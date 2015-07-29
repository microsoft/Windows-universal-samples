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
using PlaybackManager;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        // Change the string below to reflect the name of your sample.
        // This is used on the main page as the title of the sample.
        public const string FEATURE_NAME = "Audio Category Sample";

        // Change the array below to reflect the name of your scenarios.
        // This will be used to populate the list of scenarios on the main page with
        // which the user will choose the specific scenario that they are interested in.
        // These should be in the form: "Navigating to a web page".
        // The code in MainPage will take care of turning this into: "1) Navigating to a web page"
        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Movie", ClassType = typeof(Scenario1) },
            new Scenario() { Title = "Media", ClassType = typeof(Scenario2) },
            new Scenario() { Title = "Game Chat", ClassType = typeof(Scenario3) },
            new Scenario() { Title = "Speech", ClassType = typeof(Scenario4) },
            new Scenario() { Title = "Communications", ClassType = typeof(Scenario5) },
            new Scenario() { Title = "Alerts", ClassType = typeof(Scenario6) },
            new Scenario() { Title = "Sound Effects", ClassType = typeof(Scenario7) },
            new Scenario() { Title = "Game Effects", ClassType = typeof(Scenario8) },
            new Scenario() { Title = "Game Media", ClassType = typeof(Scenario9) },
            new Scenario() { Title = "Other", ClassType = typeof(Scenario10) }
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
