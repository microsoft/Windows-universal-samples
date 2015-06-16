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
        public const string FEATURE_NAME = "Message dialog C# sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Use custom commands", ClassType = typeof(CustomCommand) },
            new Scenario() { Title = "Use default close command", ClassType = typeof(DefaultCloseCommand) },
            new Scenario() { Title = "Use completed callback", ClassType = typeof(CompletedCallback) },
            new Scenario() { Title = "Use cancel command", ClassType = typeof(CancelCommand) }
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
