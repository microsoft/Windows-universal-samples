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
        public const string FEATURE_NAME = "File picker C# sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Pick a single photo",   ClassType = typeof(Scenario1_SingleFile) },
            new Scenario() { Title = "Pick multiple files",   ClassType = typeof(Scenario2_MultiFile) },
            new Scenario() { Title = "Pick a folder",         ClassType = typeof(Scenario3_SingleFolder) },
            new Scenario() { Title = "Save a file",           ClassType = typeof(Scenario4_SaveFile) },
            new Scenario() { Title = "Trigger CFU",           ClassType = typeof(Scenario5_TriggerCFU) },
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
