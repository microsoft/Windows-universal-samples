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
#include "Scenario4.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Devices::Geolocation;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Maps;

Scenario4::Scenario4()
{
    InitializeComponent();
}

void Scenario4::showSpaceNeedleButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (myMap->Is3DSupported)
    {
        myMap->Style = MapStyle::Aerial3DWithRoads;
        Geopoint^ spaceNeedleGeopoint = ref new Geopoint({ 47.6204, -122.3491 });
        MapScene^ spaceNeedleScene = MapScene::CreateFromLocationAndRadius(spaceNeedleGeopoint,
            400, /* show this many meters around */
            135, /* looking at it to the south east*/
            60 /* degrees pitch */);

        myMap->TrySetSceneAsync(spaceNeedleScene);
    }
    else
    {
        rootPage->NotifyUser(L"3D views are not supported on this device.", NotifyType::ErrorMessage);
    }
}

void Scenario4::myMap_Loaded(Object^ sender, RoutedEventArgs^ e)
{
    myMap->Center = MainPage::SeattleGeopoint;
    myMap->ZoomLevel = 12;
}
