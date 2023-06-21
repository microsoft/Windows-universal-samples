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
        public const string FEATURE_NAME = "My People notifications C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Setup", ClassType=typeof(Scenario1_Setup)},
            new Scenario() { Title="Basics", ClassType=typeof(Scenario2_Basic)},
            new Scenario() { Title="Remote assets", ClassType=typeof(Scenario3_RemoteAssets)},
            new Scenario() { Title="tel: identification", ClassType=typeof(Scenario4_TelIdentification)},
            new Scenario() { Title="remoteid: identification", ClassType=typeof(Scenario5_RemoteId)},
            new Scenario() { Title="Gifs", ClassType=typeof(Scenario6_Gifs)},
            new Scenario() { Title="Spritesheets", ClassType=typeof(Scenario7_Spritesheets)},
            new Scenario() { Title="Offset spritesheets", ClassType=typeof(Scenario8_OffsetSpritesheets)}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }

    /// <summary>
    /// Contains constant values that are used to create the sample contact. These values are later
    /// used in the notification payload to resolve the contact.
    /// </summary>
    internal static class Constants
    {
        internal const string ContactRemoteId = "{D44056FA-6C0E-47BE-B984-0974B21FF59D}";
        internal const string ContactListName = "MyPeopleNotificationsSample";
        internal const string ContactEmail = "johndoe@example.com";
        internal const string ContactPhone = "4255550123";
    }
}
