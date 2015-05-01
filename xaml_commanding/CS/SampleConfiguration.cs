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
using Commanding;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Commanding";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="AppBarButtons", ClassType=typeof(Scenario1)},
            new Scenario() { Title="Icons", ClassType=typeof(Scenario2)},
            new Scenario() { Title="Opening/Closing Events", ClassType=typeof(Scenario3)},
            new Scenario() { Title="Control the CommandBar", ClassType=typeof(Scenario4)},
            new Scenario() { Title="Multiple CommandBars", ClassType=typeof(Scenario5)},
            new Scenario() { Title="Styling", ClassType=typeof(Scenario6)},
            new Scenario() { Title="Custom Content", ClassType=typeof(Scenario7)},
            new Scenario() { Title="Command Reflow", ClassType=typeof(Scenario8)},
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
