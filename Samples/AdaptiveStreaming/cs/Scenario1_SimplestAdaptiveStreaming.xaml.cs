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

using SDKTemplate.Helpers;
using SDKTemplate.Models;
using System;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// See the README.md for discussion of this scenario.
    public sealed partial class Scenario1_SimplestAdaptiveStreaming : Page
    {
        public Scenario1_SimplestAdaptiveStreaming()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            var mp = mediaPlayerElement.MediaPlayer;
            if (mp != null)
            {
                // Explicitly disposing sources facilitates faster memory reclamation.
                mp.DisposeSource(); // From SDKTemplate.Helpers.MediaPlayerExtensions

                // Ensure MediaPlayerElement drops its reference to MediaPlayer.
                mediaPlayerElement.SetMediaPlayer(null);

                // Finally the app can also explicitly clean up the MediaPlayer.
                mp.Dispose();
            }
        }

        private void Page_OnLoaded(object sender, RoutedEventArgs e)
        {
            // NOTE: Change this to 110 to see DASH with PlayReady scenario
            const int contentIdForCMS = 1;

            AdaptiveContentModel adaptiveContentModel = MainPage.FindContentById(contentIdForCMS);

            if (adaptiveContentModel.PlayReady)
            {
                // For PlayReady, we need to create and setup MediaPlayer.MediaProtectionManager
                // All scenarios use a helper class to do this, see Helpers\PlayReadyHelper.cs
                var mp = new MediaPlayer();
                var prHelper = new PlayReadyHelper(); // From SDKTemplate.Helpers.PlayReadyHelper
                prHelper.SetUpProtectionManager(mp);
                mediaPlayerElement.SetMediaPlayer(mp);
            }

            mediaPlayerElement.Source = MediaSource.CreateFromUri(adaptiveContentModel.ManifestUri);
        }
    }
}
