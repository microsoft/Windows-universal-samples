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
        public const string FEATURE_NAME = "XamlLightUp C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Basics", ClassType=typeof(Scenario1_Basics) },
            new Scenario() { Title="Conditional execution", ClassType=typeof(Scenario2_Conditional) },
            new Scenario() { Title="Choosing controls", ClassType=typeof(Scenario3_ChooseControl) },
            new Scenario() { Title="Adaptive triggers", ClassType=typeof(Scenario4_StateTrigger) },
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
