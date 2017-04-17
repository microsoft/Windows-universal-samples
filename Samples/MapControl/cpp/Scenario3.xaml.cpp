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
#include "Scenario3.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Devices::Geolocation;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Maps;

Scenario3::Scenario3()
{
    InitializeComponent();
}

void Scenario3::MyMap_Loaded(Object^ sender, RoutedEventArgs^ e)
{
    myMap->Center = MainPage::SeattleGeopoint;
    myMap->ZoomLevel = 16;
}

void Scenario3::addXamlChildrenButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    BasicGeoposition center = myMap->Center->Position;
    auto pinUri = ref new Uri("ms-appx:///Assets/MapPin.png");
    auto places = ref new Vector<PointOfInterest^>();

    PointOfInterest^ place = ref new PointOfInterest();
    place->DisplayName = L"Place One";
    place->ImageSourceUri = pinUri;
    place->NormalizedAnchorPoint = Point{ 0.5, 1 };
    place->Location = ref new Geopoint({ center.Latitude + 0.001, center.Longitude - 0.001 });
    places->Append(place);

    place = ref new PointOfInterest();
    place->DisplayName = L"Place Two";
    place->ImageSourceUri = pinUri;
    place->NormalizedAnchorPoint = Point{ 0.5, 1 };
    place->Location = ref new Geopoint({ center.Latitude + 0.001, center.Longitude + 0.001 });
    places->Append(place);

    place = ref new PointOfInterest();
    place->DisplayName = L"Place Three";
    place->ImageSourceUri = pinUri;
    place->NormalizedAnchorPoint = Point{ 0.5, 1 };
    place->Location = ref new Geopoint({ center.Latitude - 0.001, center.Longitude - 0.001 });
    places->Append(place);

    place = ref new PointOfInterest();
    place->DisplayName = L"Place Four";
    place->ImageSourceUri = pinUri;
    place->NormalizedAnchorPoint = Point{ 0.5, 1 };
    place->Location = ref new Geopoint({ center.Latitude - 0.001, center.Longitude + 0.001 });
    places->Append(place);

    MapItems->ItemsSource = places;
}

void Scenario3::mapItemButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    Button^ buttonSender = safe_cast<Button^>(sender);
    PointOfInterest^ poi = safe_cast<PointOfInterest^>(buttonSender->DataContext);
    rootPage->NotifyUser(L"PointOfInterest clicked = " + poi->DisplayName, NotifyType::StatusMessage);
}
