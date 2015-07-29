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
        public const string FEATURE_NAME = "XAML Integration of DirectWrite Downloadable Fonts";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Using this sample app", ClassType=typeof(Scenario_SampleOverview)},
            new Scenario() { Title="Downloadable fonts overview", ClassType=typeof(Scenario_CloudFontOverview)},
            new Scenario() { Title="Document 1", ClassType=typeof(Scenario_Document1)},
            new Scenario() { Title="Document 2", ClassType=typeof(Scenario_Document2)},
            new Scenario() { Title="Document 3", ClassType=typeof(Scenario_Document3)}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
