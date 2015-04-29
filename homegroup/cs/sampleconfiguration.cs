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
        public const string FEATURE_NAME = "HomeGroup C# sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Open the file picker at HomeGroup", ClassType = typeof(OpenPicker) },
            new Scenario() { Title = "Search HomeGroup", ClassType = typeof(SearchHomeGroup) },
            new Scenario() { Title = "Stream video from Homegroup", ClassType = typeof(StreamVideo) },
            new Scenario() { Title = "Advanced search", ClassType = typeof(SearchByUser) }
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
