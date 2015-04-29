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
using VideoPlayback;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Video Playback";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title= "Playing videos", ClassType=typeof(Scenario1)},
            new Scenario() { Title= "Customizing transport controls", ClassType=typeof(Scenario2)},
            new Scenario() { Title= "Displaying in-band closed captions (MKV)", ClassType=typeof(Scenario3)},
            new Scenario() { Title= "Displaying out-of-band closed captions (MP4+SRT)", ClassType=typeof(Scenario4)},
            new Scenario() { Title= "Selecting video tracks", ClassType=typeof(Scenario5)},
            new Scenario() { Title= "Selecting audio tracks", ClassType=typeof(Scenario6)},
            new Scenario() { Title= "Playing video lists", ClassType=typeof(Scenario7)}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
