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
    { "MapControl Configuration", "SDKTemplate.Scenario1" },
    { "Adding MapElements to MapControl", "SDKTemplate.Scenario2" },
    { "Adding XAML children to MapControl", "SDKTemplate.Scenario3" },
    { "Showing 3D locations in the MapControl", "SDKTemplate.Scenario4" },
    { "Showing Streetside experience within the MapControl", "SDKTemplate.Scenario5" },
    { "Launching Maps using URI Schemes", "SDKTemplate.Scenario6" },
    { "Displaying points of interest with clustering", "SDKTemplate.Scenario7" },
};

Geopoint^ MainPage::SeattleGeopoint = ref new Geopoint({ 47.604, -122.329 });