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
using System.Linq;
using System.Threading.Tasks;
using Windows.Devices.Geolocation;
using Windows.Foundation;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Maps;

namespace SDKTemplate
{
    // A PlaceLocation represents a location in two forms:
    // Geoposition (latitude, longitude, altitude)
    // Normalized map coordinates in the unit square from (0,0) to (1,1)
    public struct PlaceLocation
    {
        public PlaceLocation(double latitude, double longitude)
        {
            Geoposition = new BasicGeoposition() { Latitude = latitude, Longitude = longitude };
            MapCoordinates = GetMapCoordinates(Geoposition);
        }
        public BasicGeoposition Geoposition { get; }
        public Point MapCoordinates { get; }

        static private Point GetMapCoordinates(BasicGeoposition geoposition)
        {
            // This formula comes from https://msdn.microsoft.com/en-us/library/bb259689.aspx

            // Clamp latitude to the range -85.05112878 to 85.05112878.
            // This keeps Y in range and avoids the singularity at the pole.
            double latitude = Math.Max(Math.Min(geoposition.Latitude, 85.05112878), -85.05112878);

            double sinLatitude = Math.Sin(latitude * Math.PI / 180.0);
            return new Point
            {
                X = (geoposition.Longitude + 180.0) / 360.0,
                Y = 0.5 - Math.Log((1.0 + sinLatitude) / (1.0 - sinLatitude)) / (4 * Math.PI)
            };
        }
    }

    public sealed class PlaceInfo
    {
        public string Name { get; set; }
        public PlaceLocation Location { get; set; }
    }

    // A cluster is a group of places that are at almost the same location.
    public sealed class Cluster
    {
        public PlaceLocation Location { get; set; }
        public IList<PlaceInfo> Places { get; private set; } = new List<PlaceInfo>();
    }

    public sealed partial class Scenario7 : Page
    {
        RandomAccessStreamReference[] numberIconReferences = new RandomAccessStreamReference[8];
        List<PlaceInfo> places = new List<PlaceInfo>();
        Dictionary<int, List<Cluster>> clustersForZoomLevel = new Dictionary<int, List<Cluster>>();

        int previousZoomLevel = 0;

        public Scenario7()
        {
            InitializeComponent();

            // Prepare the custom icons we use to represent clusters of various sizes.
            for (int i = 2; i <= 9; i++)
            {
                numberIconReferences[i - 2] = RandomAccessStreamReference.CreateFromUri(new Uri("ms-appx:///Assets/Numbers/" + i.ToString() + ".png"));
            }
        }

        private async Task LoadPlaceInfoAsync()
        {
            Uri dataUri = new Uri("ms-appx:///places.txt");

            StorageFile file = await StorageFile.GetFileFromApplicationUriAsync(dataUri);
            IList<string> lines = await FileIO.ReadLinesAsync(file);

            // In the places.txt file, each place is represented by three lines:
            // Place name, latitude, and longitude.
            for (int i = 0; i < lines.Count; i += 3)
            {
                PlaceInfo place = new PlaceInfo
                {
                    Name = lines[i],
                    Location = new PlaceLocation(double.Parse(lines[i + 1]), double.Parse(lines[i + 2]))
                };

                places.Add(place);
            }
        }

        private async void myMap_Loaded(object sender, RoutedEventArgs e)
        {
            await LoadPlaceInfoAsync();

            // Zoom in to the level that shows all our points of interest.
            GeoboundingBox boundingBox = GeoboundingBox.TryCompute(places.Select(place => place.Location.Geoposition));
            await myMap.TrySetViewBoundsAsync(boundingBox, null, MapAnimationKind.None);

            refreshMapIcons();
        }

        private void myMap_ZoomLevelChanged(MapControl sender, object args)
        {
            int intZoomLevel = (int)myMap.ZoomLevel;
            if (previousZoomLevel != intZoomLevel)
            {
                previousZoomLevel = intZoomLevel;
                refreshMapIcons();
            }
        }

        private void refreshMapIcons()
        {
            // Erase the old map icons.
            myMap.MapElements.Clear();

            // Create an icon for each cluster.
            foreach (var cluster in GetClustersForZoomLevel(previousZoomLevel))
            {
                MapIcon mapIcon = new MapIcon
                {
                    Location = new Geopoint(cluster.Location.Geoposition),
                    CollisionBehaviorDesired = MapElementCollisionBehavior.RemainVisible,
                };

                if (cluster.Places.Count > 1)
                {
                    // The cluster represents more than one place. Use a custom marker that shows
                    // how many places are represented by this cluster, and place the marker
                    // centered at the cluster.
                    mapIcon.Image = numberIconReferences[Math.Min(cluster.Places.Count, 9) - 2];
                    mapIcon.NormalizedAnchorPoint = new Point(0.5, 0.5);
                }
                else
                {
                    // The cluster represents a single place. Label the cluster with the place name.
                    mapIcon.Title = cluster.Places[0].Name;
                }
                myMap.MapElements.Add(mapIcon);
            }
        }

        private List<Cluster> GetClustersForZoomLevel(int zoomLevel)
        {
            // See if we have built the clusters for this zoom level already.
            List<Cluster> clusters;
            if (!clustersForZoomLevel.TryGetValue(zoomLevel, out clusters))
            {
                // No. Go build them, and then cache it for next time.
                clusters = BuildClustersForZoomLevel(zoomLevel);
                clustersForZoomLevel[zoomLevel] = clusters;
            }
            return clusters;
        }

        // Note that we use a very simple clustering algorithm for the purpose of this sample.
        // The algorithm is sensitive to the order of the items in the "places" list.
        private List<Cluster> BuildClustersForZoomLevel(int zoomLevel)
        {
            // For a particular zoom level, the map is 2^(zoomLevel + 8) pixels wide.
            // See https://msdn.microsoft.com/en-us/library/bb259689.aspx for more information.
            double scale = Math.Pow(2, zoomLevel + 8);

            // Say that each cluster covers a 100-pixel radius.
            const double clusterRadius = 100.0;

            double scaledclusterRadius = clusterRadius / scale;

            List<Cluster> clusters = new List<Cluster>();
            foreach (PlaceInfo place in places)
            {
                // Look for a cluster that is close to this place. If there is more than one,
                // we pick the first one arbitrarily.
                Cluster cluster = clusters.Where(c => ArePointsNear(place.Location.MapCoordinates, c.Location.MapCoordinates, scaledclusterRadius)).FirstOrDefault();

                if (cluster == null)
                {
                    // No such cluster exists yet. Make one.
                    cluster = new Cluster() { Location = place.Location };
                    clusters.Add(cluster);
                }
                // Add the place to that cluster.
                cluster.Places.Add(place);
            }

            // Update the Location of each cluster to the center of all the places within it.
            foreach (var cluster in clusters)
            {
                cluster.Location = CalculateCenter(cluster.Places);
            }

            return clusters;
        }

        // Note that this algorithm breaks down if the points straddle longitude +180/-180.
        private bool ArePointsNear(Point point1, Point point2, double threshold)
        {
            double dx = point1.X - point2.X;
            double dy = point1.Y - point2.Y;
            double distanceSquared = dx * dx + dy * dy;
            return distanceSquared < threshold * threshold;
        }

        // Calculate the center of the cluster by taking the average longitude and latitude.
        // Note that this algorithm breaks down if the points straddle longitude +180/-180.
        private PlaceLocation CalculateCenter(IList<PlaceInfo> places)
        {
            double totalLatitude = 0.0;
            double totalLongitude = 0.0;

            foreach (PlaceInfo place in places)
            {
                totalLatitude += place.Location.Geoposition.Latitude;
                totalLongitude += place.Location.Geoposition.Longitude;
            }

            return new PlaceLocation(totalLatitude / places.Count, totalLongitude / places.Count);
        }
    }
}
