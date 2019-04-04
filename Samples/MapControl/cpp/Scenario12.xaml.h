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

#include "Scenario12.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario12 sealed
    {
    public:
        Scenario12();
    private:
        MainPage ^ rootPage = MainPage::Current;

        std::vector<Platform::String^> urls;
        Windows::UI::Xaml::Controls::Maps::MapTileSource^ httpTileSource;

        void MyMapLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void AddAnimatedTilesClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ClearMapClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void HttpMapTileDataSourceUriRequested(Windows::UI::Xaml::Controls::Maps::HttpMapTileDataSource^ sender, Windows::UI::Xaml::Controls::Maps::MapTileUriRequestedEventArgs^ e);
        void PlayAnimationTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
        void PauseAnimationTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
        void StopAnimationTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
    };
}
