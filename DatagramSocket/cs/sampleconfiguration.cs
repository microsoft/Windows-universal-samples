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

using System.Collections.Generic;
using Windows.UI.Xaml.Controls;using System;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        // This is used on the main page as the title of the sample.
        public const string FEATURE_NAME = "Datagram Socket";

        // Change the array below to reflect the name of your scenarios.
        // This will be used to populate the list of scenarios on the main page with
        // which the user will choose the specific scenario that they are interested in.
        // These should be in the form: "Navigating to a web page".
        // The code in MainPage will take care of turning this into: "1) Navigating to a web page"
        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Start Datagram Listener", ClassType = typeof(DatagramSocketSample.Scenario1) },
            new Scenario() { Title = "Connect to Listener", ClassType = typeof(DatagramSocketSample.Scenario2) },
            new Scenario() { Title = "Send Data", ClassType = typeof(DatagramSocketSample.Scenario3) },
            new Scenario() { Title = "Close Socket", ClassType = typeof(DatagramSocketSample.Scenario4) }
        };
    }

    public class Scenario
    {
        public string Title { get; set; }

        public Type ClassType { get; set; }

        public override string ToString()
        {
            return Title;
        }
    }
}
