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
#include "Scenario9.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Maps;

Scenario9::Scenario9()
{
    InitializeComponent();
}

void Scenario9::MyMap_Loaded(Object^ sender, RoutedEventArgs^ e)
{
    myMap->Center = MainPage::SeattleGeopoint;
    myMap->ZoomLevel = 12;
    SetMapStyleSheet();
}

void Scenario9::SetDefaultStyleCombobox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
    // Protect against events that are raised before we are fully initialized.
    if (myMap != nullptr)
    {
        SetMapStyleSheet();
    }

}

void Scenario9::SetMapStyleSheet()
{
    switch (setDefaultStyleCombobox->SelectedIndex)
    {
    case 0:
        myMap->StyleSheet = MapStyleSheet::Aerial();
        break;
    case 1:
        myMap->StyleSheet = MapStyleSheet::AerialWithOverlay();
        break;
    case 2:
        myMap->StyleSheet = MapStyleSheet::RoadDark();
        break;
    case 3:
        myMap->StyleSheet = MapStyleSheet::RoadLight();
        break;
    case 4:
        myMap->StyleSheet = MapStyleSheet::RoadHighContrastDark();
        break;
    case 5:
        myMap->StyleSheet = MapStyleSheet::RoadHighContrastLight();
        break;
    case 6:
        myMap->StyleSheet = MapStyleSheet::ParseFromJson(R"(
                {
                    "version": "1.0",
                    "settings": {
                        "landColor": "#FFFFFF",
                        "spaceColor": "#000000"
                    },
                    "elements": {
                        "mapElement": {
                            "labelColor": "#000000",
                            "labelOutlineColor": "#FFFFFF"
                        },
                        "water": {
                            "fillColor": "#DDDDDD"
                        },
                        "area": {
                            "fillColor": "#EEEEEE"
                        },
                        "political": {
                            "borderStrokeColor": "#CCCCCC",
                            "borderOutlineColor": "#00000000"
                        }
                    }
                }
            )");
        break;
    }
}
