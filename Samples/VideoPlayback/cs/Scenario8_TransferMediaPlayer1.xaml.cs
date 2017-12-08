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

namespace SDKTemplate
{
    /// <summary>
    /// This scenario demonstrates how the lifetime of a MediaPlayer is independent of the MediaPlayerElement.
    ///
    /// The MediaPlayer created on this page can continue to play in the background, then be rebound to another
    /// scenario page.
    /// </summary>
    public sealed partial class Scenario8_TransferMediaPlayer1 : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario8_TransferMediaPlayer1()
        {
            this.InitializeComponent();
        }
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Attach any existing MediaPlayer to the MediaPlayerElement on this page.
            mediaPlayerElement.SetMediaPlayer(rootPage.commonMediaPlayer);
        }
        private void OnNavigatedFrom(object sender, RoutedEventArgs e)
        {
            // Detach the MediaPlayer from the MediaPlayerElement, but do not dispose it.
            // The MediaPlayer will continue to run, and it can be attached to another MediaPlayerElement.
            mediaPlayerElement.SetMediaPlayer(null);
        }

        private void CreateMediaPlayer_Click(object sender, RoutedEventArgs e)
        {
            if (rootPage.commonMediaPlayer == null)
            {
                rootPage.commonMediaPlayer = new Windows.Media.Playback.MediaPlayer();
                rootPage.commonMediaPlayer.AutoPlay = true;
                rootPage.commonMediaPlayer.Source = Windows.Media.Core.MediaSource.CreateFromUri(rootPage.CaptionedMediaUri);

                mediaPlayerElement.SetMediaPlayer(rootPage.commonMediaPlayer);
                rootPage.NotifyUser("Created MediaPlayer.", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("MediaPlayer already exists.", NotifyType.ErrorMessage);
            }
        }
    }
}
