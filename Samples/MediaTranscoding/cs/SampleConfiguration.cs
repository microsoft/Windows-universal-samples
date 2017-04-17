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
        public const string FEATURE_NAME = "Media Transcoding";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Transcode using default presets", ClassType=typeof(Scenario1_Default)},
            new Scenario() { Title="Transcode with custom settings", ClassType=typeof(Scenario2_Custom)},
            new Scenario() { Title="Trim and transcode", ClassType=typeof(Scenario3_Trim)}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
