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
#include "Scenario12.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Devices::Geolocation;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Maps;
using namespace Windows::UI::Xaml::Input;
using namespace std::literals;

// The data source is contributed by Iowa Environmental Mesonet of Iowa State University.
const std::wstring urlTemplate = L"http://mesonet.agron.iastate.edu/cache/tile.py/1.0.0/nexrad-n0q-{timestamp}/{zoomlevel}/{x}/{y}.png";
//The time stamps values for the IEM service for the last 50 minutes broken up into 5 minute increments.
const std::vector<std::wstring> timestamps{ L"900913-m50m", L"900913-m45m", L"900913-m40m", L"900913-m35m", L"900913-m30m", L"900913-m25m", L"900913-m20m", L"900913-m15m", L"900913-m10m", L"900913-m05m", L"900913" };

Scenario12::Scenario12()
{
    InitializeComponent();
}

void Scenario12::MyMapLoaded(Object^ sender, RoutedEventArgs^ e)
{
    myMap->Center = ref new Geopoint({ 39.1887643719098, -92.8261546188403 });
    myMap->ZoomLevel = 5;

    std::wstring timestampStr = L"{timestamp}";
    size_t index = urlTemplate.find(timestampStr);

    for (auto timestamp : timestamps)
    {
        std::wstring url(urlTemplate);
        urls.push_back(ref new String(url.replace(index, timestampStr.size(), timestamp).c_str()));
    }
}

void Scenario12::AddAnimatedTilesClick(Object^ sender, RoutedEventArgs^ e)
{
    if (httpTileSource == nullptr)
    {
        auto httpMapTileDataSource = ref new HttpMapTileDataSource();
        httpMapTileDataSource->UriRequested += ref new TypedEventHandler<HttpMapTileDataSource^, MapTileUriRequestedEventArgs^>(this, &Scenario12::HttpMapTileDataSourceUriRequested);
        httpTileSource = ref new MapTileSource(httpMapTileDataSource);
        httpTileSource->FrameCount = (int)timestamps.size();
        typedef std::chrono::duration<INT64, std::ratio_multiply<std::ratio<100>, std::nano>> timespan_units;
        httpTileSource->FrameDuration = TimeSpan{ timespan_units(500ms).count() };
        myMap->TileSources->Append(httpTileSource);
    }
}

void Scenario12::ClearMapClick(Object^ sender, RoutedEventArgs^ e)
{
    myMap->TileSources->Clear();
    httpTileSource = nullptr;
}

void Scenario12::HttpMapTileDataSourceUriRequested(HttpMapTileDataSource^ sender, MapTileUriRequestedEventArgs^ e)
{
    e->Request->Uri = ref new Uri(urls[e->FrameIndex]);
}

void Scenario12::PlayAnimationTapped(Object^ sender, TappedRoutedEventArgs^ e)
{
    if (httpTileSource != nullptr)
    {
        httpTileSource->Play();
    }
}

void Scenario12::PauseAnimationTapped(Object^ sender, TappedRoutedEventArgs^ e)
{
    if (httpTileSource != nullptr)
    {
        httpTileSource->Pause();
    }
}

void Scenario12::StopAnimationTapped(Object^ sender, TappedRoutedEventArgs^ e)
{
    if (httpTileSource != nullptr)
    {
        httpTileSource->Stop();
    }
}
