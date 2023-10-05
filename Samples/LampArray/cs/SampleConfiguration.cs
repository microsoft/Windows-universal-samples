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
using Windows.Devices.Lights;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "LampArray C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="LampArray Basics", ClassType=typeof(Scenario1_Basics)},
            new Scenario() { Title="LampArray Effects", ClassType=typeof(Scenario2_Effects)},
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }

    internal class LampArrayInfo
    {
        public LampArrayInfo(string id, string displayName, LampArray lampArray)
        {
            this.id = id;
            this.displayName = displayName;
            this.lampArray = lampArray;
        }

        public readonly string id;
        public readonly string displayName;
        public readonly LampArray lampArray;
    }
}
