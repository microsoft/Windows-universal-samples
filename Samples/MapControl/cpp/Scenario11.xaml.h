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

#include "Scenario11.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario11 sealed
    {
    public:
        Scenario11();
    private:
        MainPage^ rootPage = MainPage::Current;

        Windows::UI::Xaml::Controls::Maps::MapElementsLayer^ myLayer1;
        Windows::UI::Xaml::Controls::Maps::MapElementsLayer^ myLayer2;
        Platform::Collections::Vector<Windows::UI::Xaml::Controls::Maps::MapLayer^>^ MapLayers =
            ref new Platform::Collections::Vector<Windows::UI::Xaml::Controls::Maps::MapLayer^>();

        enum MapIconState : int
        {
            None = 0,
            Disabled = 1,
            Hover = 2,
            Selected = 4,
        };

        typedef Platform::String^ (*MapIconStateToStyleEntryState)(MapIconState state);

        static Platform::String^ ToDefaultStyleEntryState(MapIconState state);
        static Platform::String^ ToCustomStyleEntryState(MapIconState state);

        void MyMap_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void SetStyleSheetStateCombobox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void AddLayer1();
        void AddLayer2();

        static void UpdateMapElementOnPointerEntered(Windows::UI::Xaml::Controls::Maps::MapElement^ mapElement, MapIconStateToStyleEntryState toStyleEntryState);
        static void UpdateMapElementOnPointerExited(Windows::UI::Xaml::Controls::Maps::MapElement^ mapElement, MapIconStateToStyleEntryState toStyleEntryState);
        static void UpdateMapElementOnClick(Windows::UI::Xaml::Controls::Maps::MapElement^ mapElement, MapIconStateToStyleEntryState toStyleEntryState);
    };
}
