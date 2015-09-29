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
using DragAndDropSampleManaged;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "XAML Drag and Drop";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="ListView Drag and Drop and Reorder", ClassType=typeof(Scenario1_ListView)},
            new Scenario() { Title="Drag UI Customization", ClassType=typeof(Scenario2_DragUICustomization)},
            new Scenario() { Title="StartDragAsync", ClassType=typeof(Scenario3_StartDragAsync)}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
