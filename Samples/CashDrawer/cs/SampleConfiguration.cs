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
        public const string FEATURE_NAME = "Cash Drawer";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Drawer Claim and Open", ClassType=typeof(Scenario1_OpenDrawer)},
            new Scenario() { Title="Wait for Drawer Close", ClassType=typeof(Scenario2_CloseDrawer)},
            new Scenario() { Title="Drawer Retain", ClassType=typeof(Scenario3_MultipleDrawers)}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
