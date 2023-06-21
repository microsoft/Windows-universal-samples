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

#pragma once 
#include "pch.h"

namespace winrt::SDKTemplate
{
    namespace Constants
    {
        // This GUID can be any value. Use it to distinguish among different hotspots
        // managed by the same app.
        constexpr winrt::guid SampleHotspotGuid{L"4c6e8b20-b2de-472e-808b-bd423e168cd0"};

        // Information about our task that updates the hotspot metadata.
        // The EntryPoint must match the value declared in the application manifest.
        inline constexpr wchar_t MetadataUpdateTaskName[] = L"UpdateMetadataTask";
        inline constexpr wchar_t MetadataUpdateEntryPoint[] = L"BackgroundTask.UpdateMetadataTask";

        // Information about our task that turns on the hotspot.
        // The EntryPoint must match the value declared in the application manifest.
        inline constexpr wchar_t ConnectTaskName[] = L"ConnectTask";
        inline constexpr wchar_t ConnectEntryPoint[] = L"BackgroundTask.ConnectTask";

        // Information about our task that monitors whether the hotspot is nearby.
        // (We don't actually have a device watcher task, but if we did, its entry
        // point must match the value declared in the application manifest.)
        inline constexpr wchar_t DeviceWatcherTaskName[] = L"DeviceWatcherTask";
    }
}
