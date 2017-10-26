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
#include "Scenario11.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Devices::Geolocation;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Maps;

Platform::String^ Scenario11::ToDefaultStyleEntryState(MapIconState state)
{
    if (state & MapIconState::Disabled)
    {
        return MapStyleSheetEntryStates::Disabled;
    }
    else if (state & MapIconState::Selected)
    {
        return MapStyleSheetEntryStates::Selected;
    }
    else if (state & MapIconState::Hover)
    {
        return MapStyleSheetEntryStates::Hover;
    }
    else
    {
        return "";
    }
}

Platform::String^ Scenario11::ToCustomStyleEntryState(MapIconState state)
{
    if (state & MapIconState::Disabled)
    {
        return "disabled";
    }
    else if (state & MapIconState::Selected)
    {
        return "myNamespace.selected";
    }
    else if (state & MapIconState::Hover)
    {
        return "myNamespace.hover";
    }
    else
    {
        return "";
    }
}

Scenario11::Scenario11()
{
    InitializeComponent();
}

void Scenario11::MyMap_Loaded(Object^ sender, RoutedEventArgs^ e)
{
    myMap->Center = MainPage::SeattleGeopoint;
    myMap->ZoomLevel = 16;

    // Specify a style sheet.
    create_task(StorageFile::GetFileFromApplicationUriAsync(ref new Uri("ms-appx:///StyleSheets/MyCustomStates.json"))).then([](StorageFile^ file)
    {
        return create_task(FileIO::ReadTextAsync(file));
    }).then([this](String^ myStyleSheetJson)
    {
        // It is not strictly necessary to Combine since RoadLight is the default.
        myMap->StyleSheet = MapStyleSheet::Combine(ref new Vector<MapStyleSheet^>({ MapStyleSheet::RoadLight(), MapStyleSheet::ParseFromJson(myStyleSheetJson) }));

        // Populate various layers containing points and connect up states to events.
        AddLayer1();
        AddLayer2();

        // Connect the layers to the map.
        myMap->Layers = this->MapLayers;
    });
}

// The elements in layer 1 receive a default style.
void Scenario11::AddLayer1()
{
    this->myLayer1 = ref new MapElementsLayer();
    this->myLayer1->MapElements = MainPage::CreateCityBuildingsCollection();

    this->myLayer1->MapElementPointerEntered += ref new TypedEventHandler<MapElementsLayer^, MapElementsLayerPointerEnteredEventArgs^>([](MapElementsLayer^ sender, MapElementsLayerPointerEnteredEventArgs^ args)
    {
        UpdateMapElementOnPointerEntered(args->MapElement, ToDefaultStyleEntryState);
    });

    this->myLayer1->MapElementPointerExited += ref new TypedEventHandler<MapElementsLayer^, MapElementsLayerPointerExitedEventArgs^>([](MapElementsLayer^ sender, MapElementsLayerPointerExitedEventArgs^ args)
    {
        UpdateMapElementOnPointerExited(args->MapElement, ToDefaultStyleEntryState);
    });

    this->myLayer1->MapElementClick += ref new TypedEventHandler<MapElementsLayer^, MapElementsLayerClickEventArgs^>([](MapElementsLayer^ sender, MapElementsLayerClickEventArgs^ args)
    {
        UpdateMapElementOnClick(args->MapElements->GetAt(0), ToDefaultStyleEntryState);
    });

    this->MapLayers->Append(this->myLayer1);
}

// The elements in layer 2 receive a custom style.
void Scenario11::AddLayer2()
{
    this->myLayer2 = ref new MapElementsLayer();
    this->myLayer2->MapElements = MainPage::CreateCityParksCollection();

    for (auto& element : this->myLayer2->MapElements)
    {
        element->MapStyleSheetEntry = "myNamespace.myPoi";
    }

    this->myLayer2->MapElementPointerEntered += ref new TypedEventHandler<MapElementsLayer^, MapElementsLayerPointerEnteredEventArgs^>([](MapElementsLayer^ sender, MapElementsLayerPointerEnteredEventArgs^ args)
    {
        UpdateMapElementOnPointerEntered(args->MapElement, ToCustomStyleEntryState);
    });

    this->myLayer2->MapElementPointerExited += ref new TypedEventHandler<MapElementsLayer^, MapElementsLayerPointerExitedEventArgs^>([](MapElementsLayer^ sender, MapElementsLayerPointerExitedEventArgs^ args)
    {
        UpdateMapElementOnPointerExited(args->MapElement, ToCustomStyleEntryState);
    });

    this->myLayer2->MapElementClick += ref new TypedEventHandler<MapElementsLayer^, MapElementsLayerClickEventArgs^>([](MapElementsLayer^ sender, MapElementsLayerClickEventArgs^ args)
    {
        if (args->MapElements->Size > 0)
        {
            UpdateMapElementOnClick(args->MapElements->GetAt(0), ToCustomStyleEntryState);
        }
    });

    this->MapLayers->Append(this->myLayer2);
}

void Scenario11::UpdateMapElementOnPointerEntered(MapElement^ mapElement, MapIconStateToStyleEntryState toStyleEntryState)
{
    int state = mapElement->Tag != nullptr ? (int)(mapElement->Tag) : MapIconState::None;
    if (!(state & MapIconState::Hover) && !(state & MapIconState::Disabled))
    {
        state = (state | MapIconState::Hover);
        mapElement->Tag = state;
        mapElement->MapStyleSheetEntryState = toStyleEntryState((MapIconState)state);
    }
}

void Scenario11::UpdateMapElementOnPointerExited(MapElement^ mapElement, MapIconStateToStyleEntryState toStyleEntryState)
{
    int state = mapElement->Tag != nullptr ? (int)(mapElement->Tag) : MapIconState::None;
    if ((state & MapIconState::Hover) && !(state & MapIconState::Disabled))
    {
        state = (state & ~MapIconState::Hover);
        mapElement->Tag = state;
        mapElement->MapStyleSheetEntryState = toStyleEntryState((MapIconState)state);
    }
}

void Scenario11::UpdateMapElementOnClick(MapElement^ mapElement, MapIconStateToStyleEntryState toStyleEntryState)
{
    int state = mapElement->Tag != nullptr ? (int)(mapElement->Tag) : MapIconState::None;
    if (!(state & MapIconState::Disabled))
    {
        // Toggle Selected flag
        state = (state ^ MapIconState::Selected);
        mapElement->Tag = state;
        mapElement->MapStyleSheetEntryState = toStyleEntryState((MapIconState)state);
    }
}

void Scenario11::SetStyleSheetStateCombobox_SelectionChanged(Object^ sender, RoutedEventArgs^ e)
{
    // Protect against events that are raised before we are fully initialized.
    if (this->myLayer1 == nullptr) return;

    Platform::String^ selectedState = "";
    switch (setStyleSheetStateCombobox->SelectedIndex)
    {
        case 1:
            selectedState = MapStyleSheetEntryStates::Disabled;
            break;
        case 2:
            selectedState = MapStyleSheetEntryStates::Hover;
            break;
        case 3:
            selectedState = MapStyleSheetEntryStates::Selected;
            break;
        case 4:
            selectedState = "myNamespace.focused";
            break;
        case 0:
        default:
            selectedState = "";
            break;
    }

    for (auto& element : this->myLayer1->MapElements)
    {
        element->MapStyleSheetEntryState = selectedState;
    }
}
