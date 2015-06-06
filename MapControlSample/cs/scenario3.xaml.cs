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

using MapControlSample.Model;
using MapControlSample.ViewModel;
using SDKTemplate;
using Windows.Devices.Geolocation;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace MapControlSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario3 : Page
    {
        PointOfInterestsManager poiManager;
        private MainPage rootPage;

        public Scenario3()
        {
            this.InitializeComponent();
            myMap.Loaded += MyMap_Loaded;
        }

        private void MyMap_Loaded(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            myMap.Center =
               new Geopoint(new BasicGeoposition()
               {
                   //Geopoint for Seattle 
                   Latitude = 47.604,
                   Longitude = -122.329
               });
            myMap.ZoomLevel = 16;
            poiManager = new PointOfInterestsManager();
        }

        private void addXamlChildrenButton_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
          MapItems.ItemsSource = poiManager.FetchPOIs(myMap.Center.Position);
        }

        private void mapItemButton_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            var buttonSender = sender as Button;
            PointOfInterest poi = buttonSender.DataContext as PointOfInterest;            
            rootPage.NotifyUser("PointOfInterest clicked = " + poi.DisplayName, NotifyType.StatusMessage);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }
    }
}
