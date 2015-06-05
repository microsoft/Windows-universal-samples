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
using CombineCastingTech;

namespace CombineCastingTech
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "CombineCastingTech";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Combine DIAL, Miracast, Bluetooth, and DLNA using the Generic Picker", ClassType=typeof(Scenario1)},
            new Scenario() { Title="Combine DIAL, Miracast, Bluetooth, and DLNA using a custom Picker", ClassType=typeof(Scenario2)},
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}