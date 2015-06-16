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
using NfcSample;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "NFC Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="HCE Tap+Pay", ClassType=typeof(ManageCardScenario)},
            new Scenario() { Title="Card Reader", ClassType=typeof(CardReader)},
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
