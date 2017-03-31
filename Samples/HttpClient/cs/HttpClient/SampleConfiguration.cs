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
using SDKSample.HttpClientSample;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "HttpClient";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "GET Text With Cache Control", ClassType = typeof(Scenario1) },
            new Scenario() { Title = "GET Stream", ClassType = typeof(Scenario2) },
            new Scenario() { Title = "GET List", ClassType = typeof(Scenario3) },
            new Scenario() { Title = "POST Text", ClassType = typeof(Scenario4) },
            new Scenario() { Title = "POST Stream", ClassType = typeof(Scenario5) },
            new Scenario() { Title = "POST Multipart", ClassType = typeof(Scenario6) },
            new Scenario() { Title = "POST Stream With Progress", ClassType = typeof(Scenario7) },
            new Scenario() { Title = "POST Custom Content", ClassType = typeof(Scenario8) },
            new Scenario() { Title = "Get Cookies", ClassType = typeof(Scenario9) },
            new Scenario() { Title = "Set Cookie", ClassType = typeof(Scenario10) },
            new Scenario() { Title = "Delete Cookie", ClassType = typeof(Scenario11) },
            new Scenario() { Title = "Disable Cookies", ClassType = typeof(Scenario12) },
            new Scenario() { Title = "Retry Filter", ClassType = typeof(Scenario13) },
            new Scenario() { Title = "Metered Connection Filter", ClassType = typeof(Scenario14) },
            new Scenario() { Title = "Server Certificate Validation", ClassType = typeof(Scenario15) },
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
