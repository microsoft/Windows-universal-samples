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
        public const string FEATURE_NAME = "OnDemandHotspot C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Manage Hotspot", ClassType=typeof(Scenario1_ManageHotspot)},
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }

    internal sealed class Constants
    {
        // This GUID can be any value. Use it to distinguish among different hotspots
        // managed by the same app.
        public static readonly Guid SampleHotspotGuid = new Guid("4c6e8b20-b2de-472e-808b-bd423e168cd0");

        // Information about our task that updates the hotspot metadata.
        // The EntryPoint must match the value declared in the application manifest.
        public static readonly string MetadataUpdateTaskName = "UpdateMetadataTask";
        public static readonly string MetadataUpdateEntryPoint = "BackgroundTask.UpdateMetadataTask";

        // Information about our task that turns on the hotspot.
        // The EntryPoint must match the value declared in the application manifest.
        public static readonly string ConnectTaskName = "ConnectTask";
        public static readonly string ConnectEntryPoint = "BackgroundTask.ConnectTask";

        // Information about our task that monitors whether the hotspot is nearby.
        // (We don't actually have a device watcher task, but if we did, its entry
        // point must match the value declared in the application manifest.)
        public static readonly string DeviceWatcherTaskName = "DeviceWatcherTask";
    }
}
