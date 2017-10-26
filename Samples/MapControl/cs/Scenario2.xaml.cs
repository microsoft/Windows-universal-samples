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
using System.Numerics;
using Windows.Devices.Geolocation;
using Windows.Foundation;
using Windows.Foundation.Metadata;
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
        RandomAccessStreamReference mapBillboardStreamReference;
        RandomAccessStreamReference mapModelStreamReference;

        public Scenario2()
        {
            this.InitializeComponent();

            mapIconStreamReference = RandomAccessStreamReference.CreateFromUri(new Uri("ms-appx:///Assets/MapPin.png"));
            mapBillboardStreamReference = RandomAccessStreamReference.CreateFromUri(new Uri("ms-appx:///Assets/billboard.jpg"));
            mapModelStreamReference = RandomAccessStreamReference.CreateFromUri(new Uri("ms-appx:///Assets/box.3mf"));
        }

        private void MyMap_Loaded(object sender, RoutedEventArgs e)
        {
            myMap.Center = MainPage.SeattleGeopoint;
            myMap.ZoomLevel = 17;
            myMap.DesiredPitch = 45;
        }

        private void mapClearButton_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            myMap.MapElements.Clear();
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
        /// This method will create a new billboard at the center of the map with the current camera as reference
        /// </summary>
        private void mapBillboardAddButton_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            // MapBillboard scales with respect to the perspective projection of the 3D camera.
            // The reference camera determines at what view distance the billboard image appears at 1x scale.
            MapBillboard mapBillboard = new MapBillboard(myMap.ActualCamera);
            mapBillboard.Location = myMap.Center;
            mapBillboard.NormalizedAnchorPoint = new Point(0.5, 1.0);
            mapBillboard.Image = mapBillboardStreamReference;
            myMap.MapElements.Add(mapBillboard);
        }

        private async void mapElement3DAddButton_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            // Compensate for 3D Model's local coordinate system and scale
            const float AltitudeOffset = 45.0f;
            const float HeadingOffset = -90.0f;
            const float ScaleOffset = 0.1f;

            MapModel3D model = await MapModel3D.CreateFrom3MFAsync(mapModelStreamReference, MapModel3DShadingOption.Smooth);
            if (model != null)
            {
                var center = new BasicGeoposition()
                {
                    Latitude = myMap.Center.Position.Latitude,
                    Longitude = myMap.Center.Position.Longitude,
                    Altitude = AltitudeOffset,
                };

                // MapElement3D scales with respect to the perspective projection of the 3D camera
                MapElement3D mapElement3D = new MapElement3D();
                mapElement3D.Location = new Geopoint(center, AltitudeReferenceSystem.Terrain);
                mapElement3D.Model = model;
                mapElement3D.Heading = HeadingOffset + 180;
                mapElement3D.Scale = new Vector3(ScaleOffset, ScaleOffset, ScaleOffset);
                myMap.MapElements.Add(mapElement3D);
            }
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
