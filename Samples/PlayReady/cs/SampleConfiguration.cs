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
        public const string FEATURE_NAME = "PlayReady C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Reactive License Request", ClassType=typeof(Scenario1_ReactiveRequest)},
            new Scenario() { Title="Proactive License Request", ClassType=typeof(Scenario2_ProactiveRequest)},
            new Scenario() { Title="Manage HW/SW DRM", ClassType=typeof(Scenario3_HardwareDRM)},
            new Scenario() { Title="Secure Stop", ClassType=typeof(Scenario4_SecureStop)}
        };

        public static string SampleMovieURL = "http://profficialsite.origin.mediaservices.windows.net/c51358ea-9a5e-4322-8951-897d640fdfd7/tearsofsteel_4k.ism/manifest(format=mpd-time-csf)";
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
