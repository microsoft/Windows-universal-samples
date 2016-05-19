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
using SimpleInk;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "SimpleInk sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Scenario 1", ClassType=typeof(Scenario1)},
            new Scenario() { Title="Scenario 2", ClassType=typeof(Scenario2)},
            new Scenario() { Title="Scenario 3", ClassType=typeof(Scenario3)},
            new Scenario() { Title="Scenario 4", ClassType=typeof(Scenario4)},
            new Scenario() { Title="Scenario 5", ClassType=typeof(Scenario5)}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
