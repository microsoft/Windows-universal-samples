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
#include "WindowsNumerics.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Devices::Geolocation;
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls::Maps;

Scenario2::Scenario2()
{
    InitializeComponent();

    mapIconStreamReference = RandomAccessStreamReference::CreateFromUri(ref new Uri("ms-appx:///Assets/MapPin.png"));
    mapBillboardStreamReference = RandomAccessStreamReference::CreateFromUri(ref new Uri("ms-appx:///Assets/billboard.jpg"));
    mapModelStreamReference = RandomAccessStreamReference::CreateFromUri(ref new Uri("ms-appx:///Assets/box.3mf"));
}

void Scenario2::MyMap_Loaded(Object^ sender, RoutedEventArgs^ e)
{
    myMap->Center = MainPage::SeattleGeopoint;
    myMap->ZoomLevel = 17;
    myMap->DesiredPitch = 45;
}

void Scenario2::mapClearButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    myMap->MapElements->Clear();
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

void Scenario2::mapBillboardAddButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    MapBillboard^ mapBillboard = ref new MapBillboard(myMap->ActualCamera);
    mapBillboard->Location = myMap->Center;
    mapBillboard->NormalizedAnchorPoint = Point(0.5, 1.0);
    mapBillboard->Image = mapBillboardStreamReference;
    myMap->MapElements->Append(mapBillboard);
}

void Scenario2::mapElement3DAddButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    concurrency::create_task(MapModel3D::CreateFrom3MFAsync(mapModelStreamReference, MapModel3DShadingOption::Smooth)).then([this](MapModel3D^ model)
    {
        // Compensate for 3D Model's local coordinate system and scale
        const float AltitudeOffset = 45.0f;
        const float HeadingOffset = -90.0f;
        const float ScaleOffset = 0.1f;

        if (model != nullptr)
        {
            // MapElement3D scales with respect to the perspective projection of the 3D camera
            MapElement3D^ mapElement3D = ref new MapElement3D();
            mapElement3D->Location = ref new Geopoint({
                myMap->Center->Position.Latitude,
                myMap->Center->Position.Longitude,
                AltitudeOffset },
                AltitudeReferenceSystem::Terrain);
            mapElement3D->Model = model;
            mapElement3D->Heading = HeadingOffset + 180;
            mapElement3D->Scale = float3(ScaleOffset, ScaleOffset, ScaleOffset);
            myMap->MapElements->Append(mapElement3D);
        }
    });
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

