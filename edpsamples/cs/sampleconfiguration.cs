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
using EdpSample;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "EdpSample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Setup", ClassType=typeof(Scenario1)},
            new Scenario() { Title="IsIdentityManaged scenario", ClassType=typeof(Scenario2)},
            new Scenario() { Title="UI Policy scenarios", ClassType=typeof(Scenario3)},
            new Scenario() { Title="FileProtection scenarios", ClassType=typeof(Scenario4)},
            new Scenario() { Title="Revoke scenarios", ClassType=typeof(Scenario6)},
            new Scenario() { Title="Buffer protection scenarios", ClassType=typeof(Scenario7)},
            new Scenario() { Title="Stream protection scenarios", ClassType=typeof(Scenario8)},
            new Scenario() { Title="Set Clipboard Source", ClassType=typeof(Scenario9_ClipboardSource)},
            new Scenario() { Title="Set Clipboard Target", ClassType=typeof(Scenario10_ClipboardTarget)},
            new Scenario() { Title="Copy to Clipboard", ClassType=typeof(Scenario11_CpToClipboard)},
            new Scenario() { Title="Paste to New Doc", ClassType=typeof(Scenario12_CPDNoUI)},
            new Scenario() { Title="Set Share source", ClassType=typeof(Scenario13_ShareData)},
            new Scenario() { Title="Set Share target", ClassType=typeof(Scenario14_ShareTarget)},
            new Scenario() { Title="Query Copy Paste", ClassType=typeof(Scenario15_QueryCpPaste)},
            new Scenario() { Title="Data protection under Lock", ClassType=typeof(Scenario16_DPLCreateFileEvents)},
            new Scenario() { Title="Get Network Resource", ClassType=typeof(Scenario17_NetworkResource)},
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
