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
        public const string FEATURE_NAME = "ListView/GridView C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Simple ListView", ClassType=typeof(Scenario1_SimpleListView)},
            new Scenario() { Title="Simple GridView", ClassType=typeof(Scenario2_SimpleGridView)},
            new Scenario() { Title="Master Detail and Selection", ClassType=typeof(Scenario3_MasterDetail)},
            new Scenario() { Title="Edge Tapped ListView", ClassType=typeof(Scenario4_EdgeTappedListView)},
            new Scenario() { Title="Restore Scroll Position", ClassType=typeof(Scenario5_RestoreScrollPosition)},
            new Scenario() { Title="Scroll Into View", ClassType=typeof(Scenario6_ScrollIntoView)},
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
