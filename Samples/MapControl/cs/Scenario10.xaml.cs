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

using System.Collections.Generic;
using System.Collections.ObjectModel;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Maps;

namespace SDKTemplate
{
    public sealed partial class Scenario10 : Page
    {
        MapElementsLayer myLayer1;
        MapElementsLayer myLayer2;
        IList<MapLayer> myLayers = new ObservableCollection<MapLayer>();

        // This scenario creates two layers. The user can select a layer by clicking on an
        // element in that layer or by using one of the buttons.

        public Scenario10()
        {
            this.InitializeComponent();
        }

        private void MyMap_Loaded(object sender, RoutedEventArgs e)
        {
            myMap.Center = MainPage.SeattleGeopoint;
            myMap.ZoomLevel = 16;
            CreateLayers();
        }

        private void CreateLayers()
        {
            // Populate multiple layers containing points and connect up the click to events.
            AddLayer1();
            AddLayer2();

            // Style the layers so that layer 1 is selected and layer 2 is deselected.
            SelectLayer(this.myLayer1);

            // Connect the layers to the map.
            myMap.Layers = this.myLayers;
        }

        private void AddLayer1()
        {
            this.myLayer1 = new MapElementsLayer();
            this.myLayer1.MapElements = MainPage.CreateCityBuildingsCollection();
            this.myLayer1.MapElementClick += (sender, args) => SelectLayer(this.myLayer1);

            this.myLayers.Add(this.myLayer1);
        }

        private void AddLayer2()
        {
            this.myLayer2 = new MapElementsLayer();
            this.myLayer2.MapElements = MainPage.CreateCityParksCollection();
            this.myLayer2.MapElementClick += (sender, args) => SelectLayer(this.myLayer2);

            this.myLayers.Add(this.myLayer2);
        }

        private void mapElementsLayer1Button_Click(object sender, RoutedEventArgs e)
        {
            SelectLayer(this.myLayer1);
        }

        private void mapElementsLayer2Button_Click(object sender, RoutedEventArgs e)
        {
            SelectLayer(this.myLayer2);
        }

        private void SelectLayer(MapElementsLayer selectedLayer)
        {
            // Ignore layer changes if no layers have been created.
            if (selectedLayer == null) return;

            // The other layer becomes deselected.
            MapElementsLayer deselectedLayer = (selectedLayer == this.myLayer1) ? this.myLayer2 : this.myLayer1;

            // The deselected layer goes to z-index 0, and its elements appear disabled.
            deselectedLayer.ZIndex = 0;
            foreach (MapElement e in deselectedLayer.MapElements)
            {
                e.MapStyleSheetEntryState = MapStyleSheetEntryStates.Disabled;
            }

            // The selected layer goes to z-index 1, and its elements appear normal.
            selectedLayer.ZIndex = 1;
            foreach (MapElement e in selectedLayer.MapElements)
            {
                e.MapStyleSheetEntryState = "";
            }
        }
    }
}
