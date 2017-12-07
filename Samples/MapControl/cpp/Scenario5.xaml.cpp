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
#include "Scenario5.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Devices::Geolocation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Maps;

Scenario5::Scenario5()
{
    InitializeComponent();
}

void Scenario5::showStreetSideNearSpaceneedle_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (myMap->IsStreetsideSupported)
    {
        Geopoint^ spaceNeedlePoint = ref new Geopoint({ 47.6204, -122.3491 });

        concurrency::create_task(StreetsidePanorama::FindNearbyAsync(spaceNeedlePoint)).then([this](StreetsidePanorama^ panoramaNearSpaceNeedle)
        {
            if (panoramaNearSpaceNeedle != nullptr)
            {
                myMap->CustomExperience = ref new StreetsideExperience(panoramaNearSpaceNeedle);
            }
        });        
    }
    else
    {
        rootPage->NotifyUser(L"Streetside views are not supported on this device.", NotifyType::ErrorMessage);
    }
}

void Scenario5::myMap_Loaded(Object^ sender, RoutedEventArgs^ e)
{
    myMap->Center = MainPage::SeattleGeopoint;
    myMap->ZoomLevel = 12;
}
