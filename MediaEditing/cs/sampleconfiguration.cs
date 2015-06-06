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
using MediaEditingSample;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Media Editing";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Trimming and saving a clip", ClassType=typeof(Scenario1_TrimAndSaveClip)},
            new Scenario() { Title="Appending multiple clips", ClassType=typeof(Scenario2_AppendClips)},
            new Scenario() { Title="Adding background audio tracks", ClassType=typeof(Scenario3_AddAudioTracks)},
            new Scenario() { Title="Adding overlays to a clip", ClassType=typeof(Scenario4_AddOverlays)},
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
