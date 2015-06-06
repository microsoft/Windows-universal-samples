// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario1_Geotag.g.h"
#include "MainPage.xaml.h"

namespace GeotagCPPSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_Geotag sealed
    {
    public:
        Scenario1_Geotag();
    private:
        SDKTemplate::MainPage^ rootPage;
        void Button_Click_GetGeotag(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void Button_Click_SetGeotag(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void Button_Click_SetGeotagFromGeolocator(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void RequestLocationAccess();
        void GetGeotag(Windows::Storage::StorageFile^ file);
        void SetGeotagFromGeolocator(Windows::Storage::StorageFile^ file, Windows::Devices::Geolocation::Geolocator^ geolocator);
        void SetGeotag(Windows::Storage::StorageFile^ file, Windows::Devices::Geolocation::Geopoint^ geopoint);
        void LogStatus(Platform::String^ message);
        void LogError(Platform::String^ message);
    };
}
