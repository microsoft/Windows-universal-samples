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
    /// The MediaPlayerElement on this page is bound to a MediaPlayer created elsewhere.
    ///
    /// </summary>
    public sealed partial class Scenario9_TransferMediaPlayer2 : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario9_TransferMediaPlayer2()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Attach any existing MediaPlayer to the MediaPlayerElement on this page.
            mediaPlayerElement.SetMediaPlayer(rootPage.commonMediaPlayer);
        }
        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            // Detach the MediaPlayer from the MediaPlayerElement, but do not dispose it.
            // The MediaPlayer will continue to run, and it can be attached to another MediaPlayerElement.
            mediaPlayerElement.SetMediaPlayer(null);
        }

        private void DisposeMediaPlayer_Click(object sender, RoutedEventArgs e)
        {
            if(rootPage.commonMediaPlayer != null)
            {
                mediaPlayerElement.SetMediaPlayer(null);
                rootPage.commonMediaPlayer.Dispose();
                rootPage.commonMediaPlayer = null;
                rootPage.NotifyUser("Disposed of MediaPlayer.", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("MediaPlayer was already null.", NotifyType.ErrorMessage);
            }
        }
    }
}
