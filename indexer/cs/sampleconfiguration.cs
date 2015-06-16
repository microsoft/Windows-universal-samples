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
        public const string FEATURE_NAME = "Indexer C# sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Add item to the index using the ContentIndexer",              ClassType = typeof(S1_AddWithAPI) },
            new Scenario() { Title = "Update and delete indexed items using the ContentIndexer",    ClassType = typeof(S2_UpdateAndDeleteWithAPI) },
            new Scenario() { Title = "Retrieve indexed items added using the ContentIndexer",       ClassType = typeof(S3_RetrieveWithAPI) },
            new Scenario() { Title = "Check the index revision number",                             ClassType = typeof(S4_CheckIndexRevision) },
            new Scenario() { Title = "Add indexed items by using appcontent-ms files",              ClassType = typeof(S5_AddWithAppContent) },
            new Scenario() { Title = "Delete indexed appcontent-ms files",                          ClassType = typeof(S6_DeleteWithAppContent) },
            new Scenario() { Title = "Retrieve indexed properties from appcontent-ms files",        ClassType = typeof(S7_RetrieveWithAppContent) }
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
