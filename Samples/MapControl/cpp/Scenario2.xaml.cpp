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
#include "Scenario2.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Devices::Geolocation;
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls::Maps;

Scenario2::Scenario2()
{
    InitializeComponent();
    mapIconStreamReference = RandomAccessStreamReference::CreateFromUri(ref new Uri("ms-appx:///Assets/MapPin.png"));
}

void Scenario2::MyMap_Loaded(Object^ sender, RoutedEventArgs^ e)
{
    myMap->Center = MainPage::SeattleGeopoint;
    myMap->ZoomLevel = 17;
}

void Scenario2::mapIconAddButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    MapIcon^ mapIcon = ref new MapIcon();
    mapIcon->Location = myMap->Center;
    mapIcon->NormalizedAnchorPoint = Point(0.5, 1.0);
    mapIcon->Title = "My Friend";
    mapIcon->Image = mapIconStreamReference;
    mapIcon->ZIndex = 0;
    myMap->MapElements->Append(mapIcon);
}

void Scenario2::mapPolygonAddButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    double centerLatitude = myMap->Center->Position.Latitude;
    double centerLongitude = myMap->Center->Position.Longitude;
    MapPolygon^ mapPolygon = ref new MapPolygon();

    auto list = ref new Vector<BasicGeoposition, GeoEquals>();
    list->Append({ centerLatitude + 0.0005, centerLongitude - 0.001 });
    list->Append({ centerLatitude - 0.0005, centerLongitude - 0.001 });
    list->Append({ centerLatitude - 0.0005, centerLongitude + 0.001 });
    list->Append({ centerLatitude + 0.0005, centerLongitude + 0.001 });
    
    mapPolygon->Path = ref new Geopath(list);
    mapPolygon->ZIndex = 1;
    mapPolygon->FillColor = Windows::UI::Colors::Red;
    mapPolygon->StrokeColor = Windows::UI::Colors::Blue;
    mapPolygon->StrokeThickness = 3;
    mapPolygon->StrokeDashed = false;
    myMap->MapElements->Append(mapPolygon);
}

void Scenario2::mapPolylineAddButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    double centerLatitude = myMap->Center->Position.Latitude;
    double centerLongitude = myMap->Center->Position.Longitude;
    MapPolyline^ mapPolyline = ref new MapPolyline();

    auto list = ref new Vector<BasicGeoposition, GeoEquals>();
    list->Append({ centerLatitude - 0.0005, centerLongitude - 0.001 });
    list->Append({ centerLatitude + 0.0005, centerLongitude + 0.001 });

    mapPolyline->Path = ref new Geopath(list);

    mapPolyline->StrokeColor = Windows::UI::Colors::Black;
    mapPolyline->StrokeThickness = 3;
    mapPolyline->StrokeDashed = true;
    myMap->MapElements->Append(mapPolyline);
}

