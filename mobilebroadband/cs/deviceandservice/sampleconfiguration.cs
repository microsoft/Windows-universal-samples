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
using MobileBroadband;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "MobileBroadband";


        List<Scenario> scenarios = new List<Scenario>
        {
			new Scenario() { Title = "Mobile Broadband Device", ClassType = typeof(MobileBroadband.BroadbandDevice) },
			new Scenario() { Title = "Network Account Watcher", ClassType = typeof(MobileBroadband.AccountWatcher) },
            new Scenario() { Title = "Modem information (New)", ClassType = typeof(MobileBroadband.ModemInformation) },
            new Scenario() { Title = "Device Services (New)", ClassType = typeof(MobileBroadband.ModemDeviceServices) },
            new Scenario() { Title = "SIM Card (New)", ClassType = typeof(MobileBroadband.SimCard) },
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
