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

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;
using namespace Windows::Devices::Geolocation;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "MapControl configuration", "SDKTemplate.Scenario1" },
    { "Adding MapElements", "SDKTemplate.Scenario2" },
    { "Adding XAML children", "SDKTemplate.Scenario3" },
    { "Showing 3D locations", "SDKTemplate.Scenario4" },
    { "Showing Streetside experience", "SDKTemplate.Scenario5" },
    { "Launching Maps using URI Schemes", "SDKTemplate.Scenario6" },
    { "Displaying points with clustering", "SDKTemplate.Scenario7" },
    { "Find and download Offline Maps", "SDKTemplate.Scenario8" },
    { "Custom map appearance", "SDKTemplate.Scenario9" },
};

Geopoint^ MainPage::SeattleGeopoint = ref new Geopoint({ 47.604, -122.329 });