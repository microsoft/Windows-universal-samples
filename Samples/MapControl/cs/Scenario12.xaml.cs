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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Maps;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario12 : Page
    {
        // The data source is contributed by Iowa Environmental Mesonet of Iowa State University.
        private string urlTemplate = "http://mesonet.agron.iastate.edu/cache/tile.py/1.0.0/nexrad-n0q-{timestamp}/{zoomlevel}/{x}/{y}.png";
        //The time stamps values for the IEM service for the last 50 minutes broken up into 5 minute increments.
        private string[] timestamps = { "900913-m50m", "900913-m45m", "900913-m40m", "900913-m35m", "900913-m30m", "900913-m25m", "900913-m20m", "900913-m15m", "900913-m10m", "900913-m05m", "900913" };
        private MapTileSource httpTileSource = null;

        public Scenario12()
        {
            this.InitializeComponent();
        }

        private void MyMapLoaded(object sender, RoutedEventArgs e)
        {
            myMap.Center = new Geopoint(new BasicGeoposition() { Latitude = 39.1887643719098, Longitude = -92.8261546188403 });
            myMap.ZoomLevel = 5;
        }

        private void AddAnimatedTilesClick(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (httpTileSource == null)
            {
                var httpMapTileDataSource = new HttpMapTileDataSource();
                httpMapTileDataSource.UriRequested += HttpMapTileDataSourceUriRequested;
                httpTileSource = new MapTileSource()
                {
                    DataSource = httpMapTileDataSource,
                    FrameCount = timestamps.Length,
                    FrameDuration = TimeSpan.FromMilliseconds(500),
                };

                myMap.TileSources.Add(httpTileSource);
            }
        }

        private void ClearMapClick(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            myMap.TileSources.Clear();
            httpTileSource = null;
        }

        private void HttpMapTileDataSourceUriRequested(HttpMapTileDataSource sender, MapTileUriRequestedEventArgs args)
        {
            args.Request.Uri = new Uri(urlTemplate.Replace("{timestamp}", timestamps[args.FrameIndex]));
        }

        private void PlayAnimationTapped(object sender, Windows.UI.Xaml.Input.TappedRoutedEventArgs e)
        {
            if (httpTileSource != null)
            {
                httpTileSource.Play();
            }
        }

        private void PauseAnimationTapped(object sender, Windows.UI.Xaml.Input.TappedRoutedEventArgs e)
        {
            if (httpTileSource != null)
            {
                httpTileSource.Pause();
            }
        }

        private void StopAnimationTapped(object sender, Windows.UI.Xaml.Input.TappedRoutedEventArgs e)
        {
            if (httpTileSource != null)
            {
                httpTileSource.Stop();
            }
        }
    }
}
