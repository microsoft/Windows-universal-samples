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

using ScreenCasting.Data.Azure;
using ScreenCasting.Data.Common;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

namespace ScreenCasting
{
    public sealed partial class Scenario01 : Page
    {
        private MainPage rootPage;
        private VideoMetaData video;
        public Scenario01()
        {
            this.InitializeComponent();

            rootPage = MainPage.Current;

            //Subscribe to player events
            player.MediaOpened += Player_MediaOpened;
            player.MediaFailed += Player_MediaFailed;
            player.CurrentStateChanged += Player_CurrentStateChanged;

            // Get an Azure hosted video
            AzureDataProvider dataProvider = new AzureDataProvider();
            video = dataProvider.GetFromID("BigBuckBunnyTrailer");
            this.player.Source = video.VideoLink;

            //Set the source on the MediaElement
            rootPage.NotifyUser(string.Format("Opening '{0}'", video.Title), NotifyType.StatusMessage);

         

            // Use the compat version of the transport controls
            this.player.TransportControls.IsCompact = true;

            //Start Playback
            player.Play();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        #region Media Element Methods
        private void Player_CurrentStateChanged(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser(string.Format("{0} '{1}'", this.player.CurrentState, video.Title), NotifyType.StatusMessage);
        }
        private void Player_MediaFailed(object sender, ExceptionRoutedEventArgs e)
        {
            rootPage.NotifyUser(string.Format("Failed to load '{0}'", video.Title), NotifyType.ErrorMessage);
        }
        private void Player_MediaOpened(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser(string.Format("Openend '{0}'", video.Title), NotifyType.StatusMessage);
        }

        #endregion

    }
}
