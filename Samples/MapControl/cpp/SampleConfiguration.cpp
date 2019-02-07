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

using namespace Platform::Collections;
using namespace SDKTemplate;
using namespace Windows::Devices::Geolocation;
using namespace Windows::UI::Xaml::Controls::Maps;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "MapControl configuration", "SDKTemplate.Scenario1" },
    { "Adding MapElements", "SDKTemplate.Scenario2" },
    { "Adding MapLayers", "SDKTemplate.Scenario10" },
    { "Adding XAML children", "SDKTemplate.Scenario3" },
    { "Showing 3D locations", "SDKTemplate.Scenario4" },
    { "Showing Streetside experience", "SDKTemplate.Scenario5" },
    { "Launching Maps using URI Schemes", "SDKTemplate.Scenario6" },
    { "Displaying points with clustering", "SDKTemplate.Scenario7" },
    { "Find and download Offline Maps", "SDKTemplate.Scenario8" },
    { "Custom map appearance", "SDKTemplate.Scenario9" },
    { "Custom map elements appearance", "SDKTemplate.Scenario11" },
    { "Animated MapTileSource", "SDKTemplate.Scenario12" },
};

Geopoint^ MainPage::SeattleGeopoint = ref new Geopoint({ 47.604, -122.329 });

Vector<MapElement^>^ MainPage::CreateCityBuildingsCollection()
{
    auto icon1 = ref new MapIcon();
    icon1->Location = ref new Geopoint({ 47.603830, -122.329900 });
    icon1->Title = "Seattle City Hall";

    auto icon2 = ref new MapIcon();
    icon2->Location = ref new Geopoint({ 47.606709, -122.332672 });
    icon2->Title = "Seattle Public Library";

    auto icon3 = ref new MapIcon();
    icon3->Location = ref new Geopoint({ 47.604100, -122.329204 });
    icon3->Title = "Municipal Court of Seattle";

    return ref new Vector<MapElement^>({ icon1, icon2, icon3 });
}

Vector<MapElement^>^ MainPage::CreateCityParksCollection()
{
    auto icon1 = ref new MapIcon();
    icon1->Location = ref new Geopoint({ 47.608313, -122.331218 });
    icon1->Title = "Freeway Park";

    auto icon2 = ref new MapIcon();
    icon2->Location = ref new Geopoint({ 47.602029, -122.333971 });
    icon2->Title = "Pioneer Square";

    auto icon3 = ref new MapIcon();
    icon3->Location = ref new Geopoint({ 47.601990, -122.330681 });
    icon3->Title = "City Hall Park";

    auto icon4 = ref new MapIcon();
    icon4->Location = ref new Geopoint({ 47.600619, -122.324957 });
    icon4->Title = "Kobe Terrace Park";

    return ref new Vector<MapElement^>({ icon1, icon2, icon3, icon4 });
}
