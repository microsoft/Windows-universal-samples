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
            new Scenario() { Title = "Pick a single photo",   ClassType = typeof(Scenario1) },
            new Scenario() { Title = "Pick multiple files",   ClassType = typeof(Scenario2) },
            new Scenario() { Title = "Pick a folder",         ClassType = typeof(Scenario3) },
            new Scenario() { Title = "Save a file",           ClassType = typeof(Scenario4) },
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
