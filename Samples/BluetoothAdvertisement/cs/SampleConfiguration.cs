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
using BluetoothAdvertisement;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Bluetooth Low Energy Advertisement";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario()
            {
                Title = "Foreground watcher",
                ClassType = typeof(Scenario1_Watcher)
            },
            new Scenario()
            {
                Title = "Foreground publisher",
                ClassType = typeof(Scenario2_Publisher)
            },
            new Scenario()
            {
                Title = "Background watcher",
                ClassType = typeof(Scenario3_BackgroundWatcher)
            },
            new Scenario()
            {
                Title = "Background publisher",
                ClassType = typeof(Scenario4_BackgroundPublisher)
            },
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
