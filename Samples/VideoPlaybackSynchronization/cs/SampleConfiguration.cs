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
        public const string FEATURE_NAME = "Video Playback Synchronization C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Multi-camera angle", ClassType=typeof(Scenario1_MultiVideoPlayback)},
            new Scenario() { Title="Side-by-side offset sync", ClassType=typeof(Scenario2_SynchronizingPlayers)},
            new Scenario() { Title="HLS stream sync", ClassType=typeof(Scenario3_AdaptiveSync)}
        };

        public static void CleanUpMediaPlayerSource(Windows.Media.Playback.MediaPlayer mp)
        {
            if (mp?.Source != null)
            {
                var source = mp.Source as Windows.Media.Core.MediaSource;
                source?.Dispose();

                var item = mp.Source as Windows.Media.Playback.MediaPlaybackItem;
                item?.Source?.Dispose();

                mp.Source = null;
            };
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
