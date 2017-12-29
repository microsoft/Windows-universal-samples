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
#include "Scenario10.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Devices::Geolocation;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Maps;

Scenario10::Scenario10()
{
    InitializeComponent();
}

void Scenario10::MyMap_Loaded(Object^ sender, RoutedEventArgs^ e)
{
    myMap->Center = MainPage::SeattleGeopoint;
    myMap->ZoomLevel = 16;
    CreateLayers();
}

void Scenario10::CreateLayers()
{
    // Populate multiple layers containing points and connect up the click to events.
    AddLayer1();
    AddLayer2();

    // Style the layers so that layer 1 is selected and layer 2 is deselected.
    SelectLayer(this->myLayer1);

    // Connect the layers to the map.
    myMap->Layers = this->MapLayers;
}

void Scenario10::AddLayer1()
{
    this->myLayer1 = ref new MapElementsLayer();
    this->myLayer1->MapElements = MainPage::CreateCityBuildingsCollection();

    this->myLayer1->MapElementClick += ref new TypedEventHandler<MapElementsLayer^, MapElementsLayerClickEventArgs^>(this, &Scenario10::OnLayerMapElementClicked);

    this->MapLayers->Append(this->myLayer1);
}

void Scenario10::AddLayer2()
{
    this->myLayer2 = ref new MapElementsLayer();
    this->myLayer2->MapElements = MainPage::CreateCityParksCollection();

    this->myLayer2->MapElementClick += ref new TypedEventHandler<MapElementsLayer^, MapElementsLayerClickEventArgs^>(this, &Scenario10::OnLayerMapElementClicked);

    this->MapLayers->Append(this->myLayer2);
}

void Scenario10::mapElementsLayer1Button_Click(Object^ sender, RoutedEventArgs^ e)
{
    SelectLayer(this->myLayer1);
}

void Scenario10::mapElementsLayer2Button_Click(Object^ sender, RoutedEventArgs^ e)
{
    SelectLayer(this->myLayer2);
}

void Scenario10::OnLayerMapElementClicked(MapElementsLayer^ sender, MapElementsLayerClickEventArgs^ args)
{
    SelectLayer(sender);
}

void Scenario10::SelectLayer(MapElementsLayer^ selectedLayer)
{
    // Ignore layer changes if no layers have been created.
    if (selectedLayer == nullptr) return;

    // The other layer becomes deselected.
    MapElementsLayer^ deselectedLayer = (selectedLayer == this->myLayer1) ? this->myLayer2 : this->myLayer1;

    // The deselected layer goes to z-index 0, and its elements appear disabled.
    deselectedLayer->ZIndex = 0;
    for (MapElement^ element : deselectedLayer->MapElements)
    {
        element->MapStyleSheetEntryState = MapStyleSheetEntryStates::Disabled;
    }

    // The selected layer goes to z-index 1, and its elements appear normal.
    selectedLayer->ZIndex = 1;
    for (MapElement^ element : selectedLayer->MapElements)
    {
        element->MapStyleSheetEntryState = "";
    }
}
