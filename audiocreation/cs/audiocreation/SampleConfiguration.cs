//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Collections.Generic;
using Windows.UI.Xaml.Controls;
using AudioCreation;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "AudioCreation";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="File Playback", ClassType=typeof(Scenario1_FilePlayback)},
            new Scenario() { Title="Capture From Device", ClassType=typeof(Scenario2_DeviceCapture)},
            new Scenario() { Title="Using A FrameInput Node", ClassType=typeof(Scenario3_FrameInput)},
            new Scenario() { Title="Using A Submix Node", ClassType=typeof(Scenario4_Submix)},
            new Scenario() { Title="Inbox Effects", ClassType=typeof(Scenario5_InboxEffects)},
            new Scenario() { Title="Custom Effects", ClassType=typeof(Scenario6_CustomEffects)}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
