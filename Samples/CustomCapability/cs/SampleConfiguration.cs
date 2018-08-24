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
        public const string FEATURE_NAME = "CustomCapability";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Connect to an NT Service",           ClassType=typeof(MeteringData)},
            new Scenario() { Title="Connect to the FX2 Device",          ClassType=typeof(DeviceConnect) },
            new Scenario() { Title="Send IOCTLs to the device",          ClassType=typeof(DeviceIO) },
            new Scenario() { Title="Handle asynchronous device events",  ClassType=typeof(DeviceEvents) },
            new Scenario() { Title="Read and Write operations",          ClassType=typeof(DeviceReadWrite) },
            new Scenario() { Title="Custom system event trigger",        ClassType=typeof(CustomTrigger) },
            new Scenario() { Title="Firmware access",                    ClassType=typeof(FirmwareAccess) },
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
