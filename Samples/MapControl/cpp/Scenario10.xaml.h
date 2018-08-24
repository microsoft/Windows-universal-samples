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

#include "Scenario10.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario10 sealed
    {
    public:
        Scenario10();
    private:
        MainPage^ rootPage = MainPage::Current;

        Windows::UI::Xaml::Controls::Maps::MapElementsLayer^ myLayer1;
        Windows::UI::Xaml::Controls::Maps::MapElementsLayer^ myLayer2;
        Windows::UI::Xaml::Controls::Maps::MapElementsLayer^ selectedLayer;
        Platform::Collections::Vector<Windows::UI::Xaml::Controls::Maps::MapLayer^>^ MapLayers =
            ref new Platform::Collections::Vector<Windows::UI::Xaml::Controls::Maps::MapLayer^>();

        void MyMap_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void mapElementsLayer1Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void mapElementsLayer2Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnLayerMapElementClicked(Windows::UI::Xaml::Controls::Maps::MapElementsLayer^ sender, Windows::UI::Xaml::Controls::Maps::MapElementsLayerClickEventArgs^ args);

        void CreateLayers();
        void AddLayer1();
        void AddLayer2();
        void SelectLayer(Windows::UI::Xaml::Controls::Maps::MapElementsLayer^ layer);
    };
}
