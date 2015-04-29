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
using MIDI;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "MIDI";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="MIDI Device Enumeration", ClassType=typeof(Scenario1_MIDIDeviceEnumeration)},
            new Scenario() { Title="Receive MIDI Messages", ClassType=typeof(Scenario2_ReceiveMIDIMessages)},
            new Scenario() { Title="Send MIDI Messages", ClassType=typeof(Scenario3_SendMIDIMessages)}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
