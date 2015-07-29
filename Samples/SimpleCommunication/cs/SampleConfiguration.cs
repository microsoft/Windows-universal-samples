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
using SimpleCommunication;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Simple Communication";
        private static Windows.Media.MediaExtensionManager mediaExtensionMgr;

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Low Latency", ClassType=typeof(LowLatency)},
            new Scenario() { Title="Video Chat", ClassType=typeof(VideoChat)}
        };

        public void EnsureMediaExtensionManager()
        {
            if (mediaExtensionMgr == null)
            {
                mediaExtensionMgr = new Windows.Media.MediaExtensionManager();
                mediaExtensionMgr.RegisterSchemeHandler("Microsoft.Samples.SimpleCommunication.StspSchemeHandler", "stsp:");
            }
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
