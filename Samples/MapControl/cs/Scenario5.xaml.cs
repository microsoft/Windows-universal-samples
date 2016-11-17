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
using Windows.Devices.Geolocation;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Maps;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario5 : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario5()
        {
            this.InitializeComponent();
        }

        private async void showStreetSideNearSpaceneedle_Click(object sender, RoutedEventArgs e)
        {
            if (myMap.IsStreetsideSupported)
            {
                BasicGeoposition spaceNeedlePosition = new BasicGeoposition();
                spaceNeedlePosition.Latitude = 47.6204;
                spaceNeedlePosition.Longitude = -122.3491;

                Geopoint spaceNeedlePoint = new Geopoint(spaceNeedlePosition);

                StreetsidePanorama panoramaNearSpaceNeedle = await StreetsidePanorama.FindNearbyAsync(spaceNeedlePoint);

                if (panoramaNearSpaceNeedle != null)
                {
                    myMap.CustomExperience = new StreetsideExperience(panoramaNearSpaceNeedle);
                }
            }
            else
            {

                string status = "Streetside views are not supported on this device.";
                rootPage.NotifyUser(status, NotifyType.ErrorMessage);                
            }
        }

        private void myMap_Loaded(object sender, RoutedEventArgs e)
        {
            myMap.Center = MainPage.SeattleGeopoint;
            myMap.ZoomLevel = 12;
        }
    }
}
