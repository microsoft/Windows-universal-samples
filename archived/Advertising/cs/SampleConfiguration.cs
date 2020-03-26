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
        public const string FEATURE_NAME = "Advertising C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Create an ad control in markup", ClassType=typeof(Scenario1_XamlAdControl)},
            new Scenario() { Title="Create an ad control programatically", ClassType=typeof(Scenario2_ProgramaticAdControl)},
            new Scenario() { Title="Show an interstitial video ad", ClassType=typeof(Scenario3_InterstitialAd)}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
