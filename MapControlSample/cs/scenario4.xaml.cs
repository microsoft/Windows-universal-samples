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

using SDKTemplate;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Devices.Geolocation;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Maps;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace MapControlSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario4 : Page
    {
        private MainPage rootPage;

        public Scenario4()
        {
            this.InitializeComponent();
        }
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private async void showSpaceNeedleButton_Click(object sender, RoutedEventArgs e)
        {
            if (myMap.Is3DSupported)
            {
                this.myMap.Style = MapStyle.Aerial3DWithRoads;

                BasicGeoposition spaceNeedlePosition = new BasicGeoposition();
                spaceNeedlePosition.Latitude = 47.6204;
                spaceNeedlePosition.Longitude = -122.3491;

                Geopoint spaceNeedlePoint = new Geopoint(spaceNeedlePosition);

                MapScene spaceNeedleScene = MapScene.CreateFromLocationAndRadius(spaceNeedlePoint,
                                                                                    400, /* show this many meters around */
                                                                                    135, /* looking at it to the south east*/
                                                                                    60 /* degrees pitch */);

                await myMap.TrySetSceneAsync(spaceNeedleScene);
            }
            else
            {
                string status = "3D views are not supported on this device.";
                rootPage.NotifyUser(status, NotifyType.ErrorMessage);
            }

           
        }

        private void myMap_Loaded(object sender, RoutedEventArgs e)
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
    }
}
