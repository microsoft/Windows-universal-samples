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
using PrintSample;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Printing C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Basic", ClassType=typeof(Scenario1Basic)},
            new Scenario() { Title="Standard Options", ClassType=typeof(Scenario2StandardOptions)},
            new Scenario() { Title="Custom Options", ClassType=typeof(Scenario3CustomOptions)},
            new Scenario() { Title="Page Range", ClassType=typeof(Scenario4PageRange)},
            new Scenario() { Title="Photos", ClassType=typeof(Scenario5Photos)},
            new Scenario() { Title="Disable Preview", ClassType=typeof(Scenario6DisablePreview)}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}