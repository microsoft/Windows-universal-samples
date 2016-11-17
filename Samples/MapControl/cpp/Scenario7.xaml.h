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

#include "Scenario7.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    // A PlaceLocation represents a location in two forms:
    // Geoposition (latitude, longitude, altitude)
    // Normalized map coordinates in the unit square from (0,0) to (1,1)
    struct PlaceLocation
    {
        PlaceLocation(double latitude, double longitude);
        Windows::Devices::Geolocation::BasicGeoposition Geoposition;
        Windows::Foundation::Point MapCoordinates;
    private:
        static Windows::Foundation::Point GetMapCoordinates(double latitude, double longitude);
    };

    struct PlaceInfo
    {
        Platform::String^ Name;
        PlaceLocation Location;
    };

    // A cluster is a group of places that are at almost the same location.
    struct Cluster
    {
        PlaceLocation Location;
        std::vector<PlaceInfo> Places;
    };

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario7 sealed
    {
    public:
        Scenario7();
    private:
        MainPage^ rootPage = MainPage::Current;
        std::array<Windows::Storage::Streams::RandomAccessStreamReference^, 8> numberIconReferences;
        std::vector<PlaceInfo> places;
        std::map<int, std::vector<Cluster>> clustersForZoomLevel;
        int previousZoomLevel = 0;

        Concurrency::task<void> LoadPlaceInfoAsync();
        void myMap_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void myMap_ZoomLevelChanged(Windows::UI::Xaml::Controls::Maps::MapControl^ sender, Platform::Object^ args);
        void refreshMapIcons();
        const std::vector<Cluster>& GetClustersForZoomLevel(int zoomLevel);
        std::vector<Cluster> BuildClustersForZoomLevel(int zoomLevel);
        static bool ArePointsNear(Windows::Foundation::Point point1, Windows::Foundation::Point point2, double threshold);
        PlaceLocation CalculateCenter(const std::vector<PlaceInfo>& places);
    };
}
