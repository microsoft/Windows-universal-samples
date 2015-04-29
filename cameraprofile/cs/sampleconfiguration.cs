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
using CameraProfile;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "CameraProfile";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Locate Record Specific Profile", ClassType=typeof(Scenario1_SetRecordProfile)},
            new Scenario() { Title="Query Profile for Concurrency", ClassType=typeof(Scenario2_ConcurrentProfile)},
            new Scenario() { Title="Query Profile for HDR Support", ClassType=typeof(Scenario3_EnableHdrProfile)}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
