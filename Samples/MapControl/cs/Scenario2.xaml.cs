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
using Windows.Devices.Geolocation;
using Windows.Foundation;
using Windows.Storage.Streams;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Maps;
using Windows.UI.Xaml.Data;

namespace SDKTemplate
{
    public sealed partial class Scenario2 : Page
    {
        RandomAccessStreamReference mapIconStreamReference;
        public Scenario2()
        {
            this.InitializeComponent();
            mapIconStreamReference = RandomAccessStreamReference.CreateFromUri(new Uri("ms-appx:///Assets/MapPin.png"));
        }

        private void MyMap_Loaded(object sender, RoutedEventArgs e)
        {
            myMap.Center = MainPage.SeattleGeopoint;
            myMap.ZoomLevel = 17;
        }

        private void mapIconAddButton_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            MapIcon mapIcon1 = new MapIcon();
            mapIcon1.Location = myMap.Center;
            mapIcon1.NormalizedAnchorPoint = new Point(0.5, 1.0);
            mapIcon1.Title = "My Friend";
            mapIcon1.Image = mapIconStreamReference;
            mapIcon1.ZIndex = 0;
            myMap.MapElements.Add(mapIcon1);
        }

        /// <summary>
        /// This method will draw a blue rectangle around the center of the map
        /// </summary>
        private void mapPolygonAddButton_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            double centerLatitude = myMap.Center.Position.Latitude;
            double centerLongitude = myMap.Center.Position.Longitude;
            MapPolygon mapPolygon = new MapPolygon();
            mapPolygon.Path = new Geopath(new List<BasicGeoposition>() {
                new BasicGeoposition() {Latitude=centerLatitude+0.0005, Longitude=centerLongitude-0.001 },
                new BasicGeoposition() {Latitude=centerLatitude-0.0005, Longitude=centerLongitude-0.001 },
                new BasicGeoposition() {Latitude=centerLatitude-0.0005, Longitude=centerLongitude+0.001 },
                new BasicGeoposition() {Latitude=centerLatitude+0.0005, Longitude=centerLongitude+0.001 },
            });

            mapPolygon.ZIndex = 1;
            mapPolygon.FillColor = Colors.Red;
            mapPolygon.StrokeColor = Colors.Blue;
            mapPolygon.StrokeThickness = 3;
            mapPolygon.StrokeDashed = false;
            myMap.MapElements.Add(mapPolygon);
        }

        private void mapPolylineAddButton_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            double centerLatitude = myMap.Center.Position.Latitude;
            double centerLongitude = myMap.Center.Position.Longitude;
            MapPolyline mapPolyline = new MapPolyline();
            mapPolyline.Path = new Geopath(new List<BasicGeoposition>() {
                new BasicGeoposition() {Latitude=centerLatitude-0.0005, Longitude=centerLongitude-0.001 },
                new BasicGeoposition() {Latitude=centerLatitude+0.0005, Longitude=centerLongitude+0.001 },
            });

            mapPolyline.StrokeColor = Colors.Black;
            mapPolyline.StrokeThickness = 3;
            mapPolyline.StrokeDashed = true;
            myMap.MapElements.Add(mapPolyline);
        }
    }
}
