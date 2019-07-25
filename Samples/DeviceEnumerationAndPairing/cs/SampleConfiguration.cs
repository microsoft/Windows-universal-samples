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
using SDKTemplate;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Device Enumeration and Pairing C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Device Picker Common Control", ClassType=typeof(Scenario1_DevicePicker)},
            new Scenario() { Title="Enumerate and Watch Devices", ClassType=typeof(Scenario2_DeviceWatcher)},
            new Scenario() { Title="Enumerate and Watch Devices in a Background Task", ClassType=typeof(Scenario3_BackgroundDeviceWatcher)},
            new Scenario() { Title="Enumerate Snapshot of Devices", ClassType=typeof(Scenario4_Snapshot)},
            new Scenario() { Title="Get Single Device", ClassType=typeof(Scenario5_GetSingleDevice)},
            new Scenario() { Title= "Custom Filter with Additional Properties", ClassType=typeof(Scenario6_CustomFilterAddedProps)},
            new Scenario() { Title= "Request Specific DeviceInformationKind", ClassType=typeof(Scenario7_DeviceInformationKind)},
            new Scenario() { Title= "Basic Device Pairing", ClassType=typeof(Scenario8_PairDevice)},
            new Scenario() { Title= "Custom Device Pairing", ClassType=typeof(Scenario9_CustomPairDevice)}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
