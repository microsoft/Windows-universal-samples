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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using Windows.UI.Xaml.Controls.Maps;
using Windows.Devices.Geolocation;
using Windows.Foundation;
using Windows.Storage.Streams;
using System;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace MapControlSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1 : Page
    {
        private MainPage rootPage;
        
        public Scenario1()
        {
            this.InitializeComponent();            
            myMap.Loaded += MyMap_Loaded;
            myMap.MapTapped += MyMap_MapTapped;
        }

        private void MyMap_Loaded(object sender, RoutedEventArgs e)
        {
            myMap.Center =
                new Geopoint(new BasicGeoposition()
                {
                    //Geopoint for Seattle 
                    Latitude = 47.604,
                    Longitude = -122.329
                });
            myMap.ZoomLevel = 12;      
                  
        }
                
        private void MyMap_MapTapped(Windows.UI.Xaml.Controls.Maps.MapControl sender, Windows.UI.Xaml.Controls.Maps.MapInputEventArgs args)
        {
            var tappedGeoPosition = args.Location.Position;
            string status = "MapTapped at \nLatitude:" + tappedGeoPosition.Latitude + "\nLongitude: " + tappedGeoPosition.Longitude;
            rootPage.NotifyUser( status, NotifyType.StatusMessage);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private void TrafficFlowVisible_Checked(object sender, RoutedEventArgs e)
        {
            myMap.TrafficFlowVisible = true;
        }

        private void trafficFlowVisibleCheckBox_Unchecked(object sender, RoutedEventArgs e)
        {
            myMap.TrafficFlowVisible = false;
        }


       
        private void styleCombobox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            switch(styleCombobox.SelectedIndex)
            {
                case 0:
                    myMap.Style = MapStyle.None;
                    break;
                case 1:
                    myMap.Style = MapStyle.Road;
                    break;
                case 2:
                    myMap.Style = MapStyle.Aerial;
                    break;
                case 3:
                    myMap.Style = MapStyle.AerialWithRoads;
                    break;
                case 4:
                    myMap.Style = MapStyle.Terrain;
                    break;
            }            
        }
    }
}
