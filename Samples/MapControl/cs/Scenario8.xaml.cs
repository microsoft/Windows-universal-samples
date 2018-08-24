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
using Windows.Foundation.Metadata;
using Windows.Services.Maps.OfflineMaps;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario8 : Page
    {
        public Scenario8()
        {
            InitializeComponent();
        }

        private void myMap_Loaded(object sender, RoutedEventArgs e)
        {
            myMap.Center = MainPage.SeattleGeopoint;
            myMap.ZoomLevel = 12;
        }

        /// <summary>
        /// This method finds all available Offline Map packages at the map center geopoint
        /// </summary>
        private async void findAtCenterButton_Click(object sender, RoutedEventArgs e)
        {
            resultsGrid.Children.Clear();
            resultsGrid.RowDefinitions.Clear();
            resultStatusTextBlock.Text = "";

            OfflineMapPackageQueryResult result = await OfflineMapPackage.FindPackagesAsync(myMap.Center);

            if (result.Status != OfflineMapPackageQueryStatus.Success)
            {
                resultStatusTextBlock.Text = "Error: " + result.Status;
            }
            else if (result.Packages.Count == 0)
            {
                resultStatusTextBlock.Text = "No maps found";
            }
            else
            {
                foreach (OfflineMapPackage package in result.Packages)
                {
                    addToResultGrid(package);
                }
            }
        }

        private void addToResultGrid(OfflineMapPackage package)
        {
            // a descriptive name for the package
            // ex: "Washington, USA (160 MB)"
            string descriptiveName = package.DisplayName + ", " + package.EnclosingRegionName + " (" + nearestMegabyte(package.EstimatedSizeInBytes) + " MB)";

            // current download status (Downloaded, NotDownloaded, Downloading, etc)
            string downloadStatus = package.Status.ToString();

            TextBlock packageNameTextBlock = new TextBlock()
            {
                Text = descriptiveName,
                Margin = new Thickness(5),
                VerticalAlignment = VerticalAlignment.Center,
            };

            TextBlock packageStatusTextBlock = new TextBlock()
            {
                Text = downloadStatus,
                Margin = new Thickness(5),
                VerticalAlignment = VerticalAlignment.Center,
            };

            Button packageDownloadButton = new Button()
            {
                Content = "Download",
                IsEnabled = canDownload(package),
            };

            // event handler for Download button
            packageDownloadButton.Click += async (sender, e) =>
            {
                // queue up the package for download
                // requires offlineMapsManagement capability (see Package.appxmanifest)
                OfflineMapPackageStartDownloadResult result = await package.RequestStartDownloadAsync();

                // show any error
                if (result.Status != OfflineMapPackageStartDownloadStatus.Success)
                {
                    packageStatusTextBlock.Text = result.Status.ToString();
                }
            };

            // listen for package status changes and update the UI (download status, download button enable/disable)
            package.StatusChanged += async (sender, args) =>
            {
                // event might not be on the UI thread, so marshal if we need to update UI
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    packageStatusTextBlock.Text = package.Status.ToString();
                    packageDownloadButton.IsEnabled = canDownload(package);
                });
            };

            Grid.SetRow(packageNameTextBlock, resultsGrid.RowDefinitions.Count);
            Grid.SetColumn(packageNameTextBlock, 0);

            Grid.SetRow(packageStatusTextBlock, resultsGrid.RowDefinitions.Count);
            Grid.SetColumn(packageStatusTextBlock, 1);

            Grid.SetRow(packageDownloadButton, resultsGrid.RowDefinitions.Count);
            Grid.SetColumn(packageDownloadButton, 2);

            resultsGrid.RowDefinitions.Add(new RowDefinition() { Height = GridLength.Auto });

            resultsGrid.Children.Add(packageNameTextBlock);
            resultsGrid.Children.Add(packageStatusTextBlock);
            resultsGrid.Children.Add(packageDownloadButton);
        }

        private static long nearestMegabyte(ulong bytes)
        {
            return (long)Math.Round(bytes / (1024.0 * 1024.0));
        }

        private static bool canDownload(OfflineMapPackage package)
        {
            // we can start a download if the package isn't downloaded or it's currently being deleted
            return package.Status == OfflineMapPackageStatus.NotDownloaded || package.Status == OfflineMapPackageStatus.Deleting;
        }
    }
}
