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

#include "Scenario3.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::UI::Xaml::Data::Bindable]
    public ref class PointOfInterest sealed
    {
    public:
        property Platform::String^ DisplayName;
        property Windows::Devices::Geolocation::Geopoint^ Location;
        property Windows::Foundation::Uri^ ImageSourceUri;
        property Windows::Foundation::Point NormalizedAnchorPoint;
    };

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3 sealed
    {
    public:
        Scenario3();
    private:
        MainPage^ rootPage = MainPage::Current;
        void MyMap_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void addXamlChildrenButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void mapItemButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
