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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    public class PointOfInterest
    {
        public string DisplayName { get; set; }
        public Geopoint Location { get; set; }
        public Uri ImageSourceUri { get; set; }
        public Point NormalizedAnchorPoint { get; set; }
    }

    public sealed partial class Scenario3 : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario3()
        {
            this.InitializeComponent();
        }

        private void MyMap_Loaded(object sender, RoutedEventArgs e)
        {
            myMap.Center = MainPage.SeattleGeopoint;
            myMap.ZoomLevel = 16;
        }

        private void addXamlChildrenButton_Click(object sender, RoutedEventArgs e)
        {
            BasicGeoposition center = myMap.Center.Position;
            var pinUri = new Uri("ms-appx:///Assets/MapPin.png");
            MapItems.ItemsSource = new List<PointOfInterest>()
            {
                new PointOfInterest()
                {
                    DisplayName = "Place One",
                    ImageSourceUri = pinUri,
                    NormalizedAnchorPoint = new Point(0.5, 1),
                    Location = new Geopoint(new BasicGeoposition()
                    {
                        Latitude = center.Latitude + 0.001,
                        Longitude = center.Longitude - 0.001
                    })
                },
                new PointOfInterest()
                {
                    DisplayName = "Place Two",
                    ImageSourceUri = pinUri,
                    NormalizedAnchorPoint = new Point(0.5, 1),
                    Location = new Geopoint(new BasicGeoposition()
                    {
                        Latitude = center.Latitude + 0.001,
                        Longitude = center.Longitude + 0.001
                    })
                },
                new PointOfInterest()
                {
                    DisplayName = "Place Three",
                    ImageSourceUri = pinUri,
                    NormalizedAnchorPoint = new Point(0.5, 1),
                    Location = new Geopoint(new BasicGeoposition()
                    {
                        Latitude = center.Latitude - 0.001,
                        Longitude = center.Longitude - 0.001
                    })
                },
                new PointOfInterest()
                {
                    DisplayName = "Place Four",
                    ImageSourceUri = pinUri,
                    NormalizedAnchorPoint = new Point(0.5, 1),
                    Location = new Geopoint(new BasicGeoposition()
                    {
                        Latitude = center.Latitude - 0.001,
                        Longitude = center.Longitude + 0.001
                    })
                }
            };
        }

        private void mapItemButton_Click(object sender, RoutedEventArgs e)
        {
            var buttonSender = sender as Button;
            PointOfInterest poi = buttonSender.DataContext as PointOfInterest;            
            rootPage.NotifyUser("PointOfInterest clicked = " + poi.DisplayName, NotifyType.StatusMessage);
        }
    }
}
