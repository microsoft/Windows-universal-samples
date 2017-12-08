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
#include "Scenario7.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Devices::Geolocation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Maps;

constexpr static long double Pi = 3.14159265358979323846264338327950288L;

PlaceLocation::PlaceLocation(double latitude, double longitude) :
    Geoposition{ latitude, longitude },
    MapCoordinates(GetMapCoordinates(latitude, longitude))
{
}

Point PlaceLocation::GetMapCoordinates(double latitude, double longitude)
{
    // This formula comes from https://msdn.microsoft.com/en-us/library/bb259689.aspx

    // Clamp latitude to the range -85.05112878 to 85.05112878.
    // This keeps Y in range and avoids the singularities at the poles.
    latitude = (std::max)((std::min)(latitude, 85.05112878), -85.05112878);

    double sinLatitude = sin(latitude * Pi / 180.0);
    return Point{
        static_cast<float>((longitude + 180.0) / 360.0),
        static_cast<float>(0.5 - log((1.0 + sinLatitude) / (1.0 - sinLatitude)) / (4 * Pi))
    };
}

Scenario7::Scenario7()
{
    InitializeComponent();

    // Prepare the custom icons we use to represent clusters of various sizes.
    for (int i = 2; i <= 9; i++)
    {
        numberIconReferences[i - 2] = RandomAccessStreamReference::CreateFromUri(ref new Uri("ms-appx:///Assets/Numbers/" + i.ToString() + ".png"));
    }
}

task<void> Scenario7::LoadPlaceInfoAsync()
{
    auto dataUri = ref new Uri("ms-appx:///places.txt");

    return create_task(StorageFile::GetFileFromApplicationUriAsync(dataUri)).then([](StorageFile^ file)
    {
        return create_task(FileIO::ReadLinesAsync(file));
    }).then([this](IVector<String^>^ lines)
    {
        // In the places.txt file, each place is represented by three lines:
        // Place name, latitude, and longitude.
        for (unsigned int i = 0; i < lines->Size; i += 3)
        {
            places.push_back(PlaceInfo{
                lines->GetAt(i),
                PlaceLocation(_wtof(lines->GetAt(i + 1)->Data()), _wtof(lines->GetAt(i + 2)->Data())) });
        }
    });
}

void Scenario7::myMap_Loaded(Object^ sender, RoutedEventArgs^ e)
{
    LoadPlaceInfoAsync().then([this]()
    {
        // Zoom in to the level that shows all our points of interest.
        std::vector<BasicGeoposition> positions(places.size());
        std::transform(places.begin(), places.end(), positions.begin(),
            [](const PlaceInfo& place) { return place.Location.Geoposition; });
        GeoboundingBox^ boundingBox = GeoboundingBox::TryCompute(ref new Vector<BasicGeoposition, GeoEquals>(positions));
        return create_task(myMap->TrySetViewBoundsAsync(boundingBox, nullptr, MapAnimationKind::None));
    }, task_continuation_context::get_current_winrt_context()).then([this](bool)
    {
        refreshMapIcons();
    }, task_continuation_context::get_current_winrt_context());
}

void Scenario7::myMap_ZoomLevelChanged(MapControl^ sender, Object^ args)
{
    int intZoomLevel = (int)myMap->ZoomLevel;
    if (previousZoomLevel != intZoomLevel)
    {
        previousZoomLevel = intZoomLevel;
        refreshMapIcons();
    }
}

void Scenario7::refreshMapIcons()
{
    // Erase the old map icons.
    myMap->MapElements->Clear();

    // Create an icon for each cluster.
    for (const Cluster& cluster : GetClustersForZoomLevel(previousZoomLevel))
    {
        MapIcon^ mapIcon = ref new MapIcon();
        mapIcon->Location = ref new Geopoint(cluster.Location.Geoposition);
        mapIcon->CollisionBehaviorDesired = MapElementCollisionBehavior::RemainVisible;

        if (cluster.Places.size() > 1)
        {
            // The cluster represents more than one place. Use a custom marker that shows
            // how many places are represented by this cluster, and place the marker
            // centered at the cluster.
            mapIcon->Image = numberIconReferences[std::min<size_t>(cluster.Places.size(), 9) - 2];
            mapIcon->NormalizedAnchorPoint = Point(0.5, 0.5);
        }
        else
        {
            // The cluster represents a single place. Label the cluster with the place name.
            mapIcon->Title = cluster.Places[0].Name;
        }
        myMap->MapElements->Append(mapIcon);
    }
}

const std::vector<Cluster>& Scenario7::GetClustersForZoomLevel(int zoomLevel)
{
    // See if we have built the clusters for this zoom level already.
    auto found = clustersForZoomLevel.find(zoomLevel);
    if (found != clustersForZoomLevel.end())
    {
        // Yes, return it.
        return found->second;
    }

    // No. Go build the clusters, cache them for next time, and return them.
    return clustersForZoomLevel[zoomLevel] = BuildClustersForZoomLevel(zoomLevel);
}

// Note that we use a very simple clustering algorithm for the purpose of this sample.
// The algorithm is sensitive to the order of the items in the "places" list.
std::vector<Cluster> Scenario7::BuildClustersForZoomLevel(int zoomLevel)
{
    // For a particular zoom level, the map is 2^(zoomLevel + 8) pixels wide.
    // See https://msdn.microsoft.com/en-us/library/bb259689.aspx for more information.
    double scale = pow(2, zoomLevel + 8);

    // Say that each cluster covers a 100-pixel radius.
    const double clusterRadius = 100.0;

    double scaledclusterRadius = clusterRadius / scale;

    std::vector<Cluster> clusters;
    for (const PlaceInfo& place : places)
    {
        // Look for a cluster that is close to this place. If there is more than one,
        // we pick the first one arbitrarily.
        auto found = std::find_if(clusters.begin(), clusters.end(),
            [place, scaledclusterRadius](const Cluster& cluster)
        {
            return ArePointsNear(place.Location.MapCoordinates, cluster.Location.MapCoordinates, scaledclusterRadius);
        });

        if (found == clusters.end())
        {
            // No such cluster exists yet. Make one.
            found = clusters.insert(clusters.end(), Cluster{ place.Location });
        }
        // Add the place to that cluster.
        found->Places.push_back(place);
    }

    // Update the Location of each cluster to the center of all the places within it.
    for (Cluster& cluster : clusters)
    {
        cluster.Location = CalculateCenter(cluster.Places);
    }

    return clusters;
}

// Note that this algorithm breaks down if the points straddle longitude +180/-180.
bool Scenario7::ArePointsNear(Point point1, Point point2, double threshold)
{
    double dx = point1.X - point2.X;
    double dy = point1.Y - point2.Y;
    double distanceSquared = dx * dx + dy * dy;
    return distanceSquared < threshold * threshold;
}

// Calculate the center of the cluster by taking the average longitude and latitude.
// Note that this algorithm breaks down if the points straddle longitude +180/-180.
PlaceLocation Scenario7::CalculateCenter(const std::vector<PlaceInfo>& places)
{
    double totalLatitude = 0.0;
    double totalLongitude = 0.0;

    for (const PlaceInfo& place : places)
    {
        totalLatitude += place.Location.Geoposition.Latitude;
        totalLongitude += place.Location.Geoposition.Longitude;
    }

    return PlaceLocation(totalLatitude / places.size(), totalLongitude / places.size());
}
