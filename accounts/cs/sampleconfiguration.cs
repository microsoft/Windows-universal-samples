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
using Accounts;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Accounts";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Single Microsoft account", ClassType=typeof(SingleMicrosoftAccountScenario)},
            new Scenario() { Title="Single account of any provider", ClassType=typeof(SingleAccountScenario)},
            new Scenario() { Title="Multiple accounts", ClassType=typeof(MultipleAccountsScenario)},
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
