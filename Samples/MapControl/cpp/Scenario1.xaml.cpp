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
#include "Scenario1.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Devices::Geolocation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Maps;

Scenario1::Scenario1()
{
    InitializeComponent();
}

void Scenario1::MyMap_Loaded(Object^ sender, RoutedEventArgs^ e)
{
    myMap->Center = MainPage::SeattleGeopoint;
    myMap->ZoomLevel = 12;
}

void Scenario1::MyMap_MapTapped(MapControl^ sender, MapInputEventArgs^ args)
{
    BasicGeoposition tappedGeoPosition = args->Location->Position;
    String^ status = "MapTapped at Latitude:" + tappedGeoPosition.Latitude + ", Longitude: " + tappedGeoPosition.Longitude;
    rootPage->NotifyUser(status, NotifyType::StatusMessage);
}

void Scenario1::TrafficFlowVisible_Checked(Object^ sender, RoutedEventArgs^ e)
{
    myMap->TrafficFlowVisible = true;
}

void Scenario1::trafficFlowVisibleCheckBox_Unchecked(Object^ sender, RoutedEventArgs^ e)
{
    myMap->TrafficFlowVisible = false;
}

void Scenario1::styleCombobox_SelectionChanged(Object^ sender, SelectionChangedEventArgs^ e)
{
    switch (styleCombobox->SelectedIndex)
    {
    case 0:
        myMap->Style = MapStyle::None;
        break;
    case 1:
        myMap->Style = MapStyle::Road;
        break;
    case 2:
        myMap->Style = MapStyle::Aerial;
        break;
    case 3:
        myMap->Style = MapStyle::AerialWithRoads;
        break;
    case 4:
        myMap->Style = MapStyle::Terrain;
        break;
    }
}