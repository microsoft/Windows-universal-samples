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

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Maps;

namespace SDKTemplate
{
    public sealed partial class Scenario11 : Page
    {
        MapElementsLayer myLayer1;
        MapElementsLayer myLayer2;
        IList<MapLayer> myLayers = new ObservableCollection<MapLayer>();

        [Flags]
        enum MapIconState
        {
            None = 0,
            Disabled = 1,
            Hover = 2,
            Selected = 4,
        }

        delegate string MapIconStateToStyleEntryState(MapIconState state);

        static string ToDefaultStyleEntryState(MapIconState state)
        {
            if (state.HasFlag(MapIconState.Disabled))
            {
                return MapStyleSheetEntryStates.Disabled;
            }
            else if (state.HasFlag(MapIconState.Selected))
            {
                return MapStyleSheetEntryStates.Selected;
            }
            else if (state.HasFlag(MapIconState.Hover))
            {
                return MapStyleSheetEntryStates.Hover;
            }
            else
            {
                return string.Empty;
            }
        }

        static string ToCustomStyleEntryState(MapIconState state)
        {
            if (state.HasFlag(MapIconState.Disabled))
            {
                return "disabled";
            }
            else if (state.HasFlag(MapIconState.Selected))
            {
                return "myNamespace.selected";
            }
            else if (state.HasFlag(MapIconState.Hover))
            {
                return "myNamespace.hover";
            }
            else
            {
                return string.Empty;
            }
        }

        public Scenario11()
        {
            this.InitializeComponent();
        }

        private async void MyMap_Loaded(object sender, RoutedEventArgs e)
        {
            myMap.Center = MainPage.SeattleGeopoint;
            myMap.ZoomLevel = 16;

            // Specify a style sheet.  It is not strictly necessary to Combine since RoadLight is the default.
            var myStyleSheetFile = await StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///StyleSheets/MyCustomStates.json"));
            string myStyleSheetJson = await FileIO.ReadTextAsync(myStyleSheetFile);

            myMap.StyleSheet = MapStyleSheet.Combine(new MapStyleSheet[]
            {
                MapStyleSheet.RoadLight(),
                MapStyleSheet.ParseFromJson(myStyleSheetJson)
            });

            // Populate various layers containing points and connect up states to events.
            AddLayer1();
            AddLayer2();

            // Connect the layers to the map.
            myMap.Layers = this.myLayers;
        }

        // The elements in layer 1 receive a default style.
        private void AddLayer1()
        {
            this.myLayer1 = new MapElementsLayer();
            this.myLayer1.MapElements = MainPage.CreateCityBuildingsCollection();
            this.myLayer1.MapElementPointerEntered += (sender, args) => UpdateMapElementOnPointerEntered(args.MapElement, ToDefaultStyleEntryState);
            this.myLayer1.MapElementPointerExited += (sender, args) => UpdateMapElementOnPointerExited(args.MapElement, ToDefaultStyleEntryState);
            this.myLayer1.MapElementClick += (sender, args) => UpdateMapElementOnClick(args.MapElements[0], ToDefaultStyleEntryState);

            this.myLayers.Add(this.myLayer1);
        }

        // The elements in layer 2 receive a custom style.
        private void AddLayer2()
        {
            this.myLayer2 = new MapElementsLayer();
            this.myLayer2.MapElements = MainPage.CreateCityParksCollection();

            foreach (MapElement element in myLayer2.MapElements)
            {
                element.MapStyleSheetEntry = "myNamespace.myPoi";
            }

            this.myLayer2.MapElementPointerEntered += (sender, args) => UpdateMapElementOnPointerEntered(args.MapElement, ToCustomStyleEntryState);
            this.myLayer2.MapElementPointerExited += (sender, args) => UpdateMapElementOnPointerExited(args.MapElement, ToCustomStyleEntryState);
            this.myLayer2.MapElementClick += (sender, args) => UpdateMapElementOnClick(args.MapElements[0], ToCustomStyleEntryState);

            this.myLayers.Add(this.myLayer2);
        }

        static void UpdateMapElementOnPointerEntered(MapElement mapElement, MapIconStateToStyleEntryState toStyleEntryState)
        {
            var state = (MapIconState)(mapElement.Tag ?? MapIconState.None);
            if (!state.HasFlag(MapIconState.Hover) && !state.HasFlag(MapIconState.Disabled))
            {
                state |= MapIconState.Hover;
                mapElement.Tag = state;
                mapElement.MapStyleSheetEntryState = toStyleEntryState(state);
            }
        }

        static void UpdateMapElementOnPointerExited(MapElement mapElement, MapIconStateToStyleEntryState toStyleEntryState)
        {
            var state = (MapIconState)(mapElement.Tag ?? MapIconState.None);
            if (state.HasFlag(MapIconState.Hover) && !state.HasFlag(MapIconState.Disabled))
            {
                state &= ~MapIconState.Hover;
                mapElement.Tag = state;
                mapElement.MapStyleSheetEntryState = toStyleEntryState(state);
            }
        }

        static void UpdateMapElementOnClick(MapElement mapElement, MapIconStateToStyleEntryState toStyleEntryState)
        {
            var state = (MapIconState)(mapElement.Tag ?? MapIconState.None);
            if (!state.HasFlag(MapIconState.Disabled))
            {
                // Toggle Selected flag (clear bit then OR-in toggled value)
                state = (state & ~MapIconState.Selected) | (state ^ MapIconState.Selected);
                mapElement.Tag = state;
                mapElement.MapStyleSheetEntryState = toStyleEntryState(state);
            }
        }

        private void SetStyleSheetStateCombobox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            // Protect against events that are raised before we are fully initialized.
            if (this.myLayer1 == null) return;

            string selectedState = string.Empty;
            switch (setStyleSheetStateCombobox.SelectedIndex)
            {
                case 1:
                    selectedState = MapStyleSheetEntryStates.Disabled;
                    break;
                case 2:
                    selectedState = MapStyleSheetEntryStates.Hover;
                    break;
                case 3:
                    selectedState = MapStyleSheetEntryStates.Selected;
                    break;
                case 4:
                    selectedState = "myNamespace.focused";
                    break;
                case 0:
                default:
                    selectedState = "";
                    break;
            }

            foreach (MapElement element in this.myLayer1.MapElements)
            {
                element.MapStyleSheetEntry = selectedState;
            }
        }
    }
}
