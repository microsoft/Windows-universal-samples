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

using SDKSample;
using System;
using System.Collections.Generic;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Projection";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Start projecting", ClassType=typeof(Scenario1)},
            new Scenario() { Title="Select projector and project", ClassType=typeof(Scenario2)},
            new Scenario() { Title="Custom display selection and project", ClassType=typeof(Scenario3)}
        };

        // Keep track of the view that's being projected
        public SecondaryViewsHelpers.ViewLifetimeControl ProjectionViewPageControl;
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
