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
using MapControlSample;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "MapControlSample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="MapControl Configuration", ClassType=typeof(Scenario1)},
            new Scenario() { Title="Adding MapElements to MapControl", ClassType=typeof(Scenario2)},
            new Scenario() { Title="Adding XAML children to MapControl", ClassType=typeof(Scenario3)}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
