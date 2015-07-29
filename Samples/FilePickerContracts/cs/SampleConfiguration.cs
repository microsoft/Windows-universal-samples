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
using FilePickerContracts;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "File picker contracts C# sample";
        public const string APP_TITLE = "File Picker Contracts C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "File Open Picker contract", ClassType = typeof(FilePickerContracts.MainPage_PickFile) },
            new Scenario() { Title = "File Save Picker contract", ClassType = typeof(FilePickerContracts.MainPage_SaveFile) },
            new Scenario() { Title = "Cached File Updater contract", ClassType = typeof(FilePickerContracts.MainPage_CachedFile) },
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }

    internal class Status
    {
        internal static readonly string FileAdded = "File added to the basket.";
        internal static readonly string FileRemoved = "File removed from the basket.";
        internal static readonly string FileAddFailed = "Couldn't add file to the basket.";
    }
}
