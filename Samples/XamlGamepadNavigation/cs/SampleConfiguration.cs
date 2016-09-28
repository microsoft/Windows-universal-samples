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
        public const string FEATURE_NAME = "Resize app view";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Directional navigation", ClassType=typeof(Scenario1_Directions)},
            new Scenario() { Title="Engaging focus", ClassType=typeof(Scenario2_Engage)},
            new Scenario() { Title="Legacy controls", ClassType=typeof(Scenario3_Legacy)},
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }

    partial class App
    {
        partial void Construct()
        {
            // Setting RequiresPointerMode to WhenRequested sets the default behavior for
            // the entire app to enter mouse mode only when requested. This is typically
            // set in the App.xaml markup, but we do it here explicitly in code so that
            // the step is easier to see.
            this.RequiresPointerMode = Windows.UI.Xaml.ApplicationRequiresPointerMode.WhenRequested;
        }
    }

}
