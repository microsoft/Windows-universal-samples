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
using System.Numerics;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.UI.Composition;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Hosting;
using Windows.UI.Xaml.Navigation;

namespace VideoPlayback
{
    /// <summary>
    /// Demonstrates basic rendering of a MediaPlayer to a Composition surface.
    ///
    /// This is a copy of scenario 1, but with UI Composition techniques.
    ///
    /// Not shown in this sample are re-sizing a button to the video size, nor
    /// resizing video to button size, nor resizing operations as the UI changes.
    /// </summary>
    public sealed partial class Scenario8 : Page
    {
        private MainPage rootPage;
        private MediaPlayer mediaPlayer;

        public Scenario8()
        {
            this.InitializeComponent();
            Loaded += Scenario8_Loaded;
        }

        private void Scenario8_Loaded(object sender, RoutedEventArgs e)
        {
            mediaPlayer = new MediaPlayer();
            mediaPlayer.Source = MediaSource.CreateFromUri(new Uri("https://mediaplatstorage1.blob.core.windows.net/windows-universal-samples-media/elephantsdream-clip-h264_sd-aac_eng-aac_spa-aac_eng_commentary-srt_eng-srt_por-srt_swe.mkv"));
            mediaPlayer.Play();
            BindMediaPlayerToUIElement(mediaPlayer, mediaPlayerButton);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            // Dispose the media player so it doesn't continue to play audio
            // after the page is unloaded
            mediaPlayer.Dispose();

            base.OnNavigatedFrom(e);
        }

        private void mediaPlayerButton_Click(object sender, RoutedEventArgs e)
        {
            if (mediaPlayer.PlaybackSession.PlaybackState == MediaPlaybackState.Playing)
                mediaPlayer.Pause();
            else if (mediaPlayer.PlaybackSession.PlaybackState == MediaPlaybackState.Paused)
                mediaPlayer.Play();
        }

        private void BindMediaPlayerToUIElement(MediaPlayer mediaPlayer, UIElement uiElement)
        {
            // Get the backing composition visual for the passed in UIElement
            Visual hostVisual = ElementCompositionPreview.GetElementVisual(uiElement);

            // Get the compositor the visual belongs to
            Compositor compositor = hostVisual.Compositor;

            // Create a new container and make it a child of the element
            ContainerVisual container = compositor.CreateContainerVisual();
            ElementCompositionPreview.SetElementChildVisual(uiElement, container);

            // Add a new sprite visual to paint the player onto
            var playerVisual = compositor.CreateSpriteVisual();
            playerVisual.Size = uiElement.RenderSize.ToVector2();
            container.Children.InsertAtTop(playerVisual);

            // Get the player's surface
            var mediaSurface = mediaPlayer.GetSurface(compositor);
            var compositionSurface = mediaSurface.CompositionSurface;

            // Convert the surface to a brush used to paint the visual
            playerVisual.Brush = compositor.CreateSurfaceBrush(compositionSurface);
        }

        /// <summary>
        /// Handles the pick file button click event to load a video.
        /// </summary>
        private async void pickFileButton_Click(object sender, RoutedEventArgs e)
        {
            // Clear previous returned file name, if it exists, between iterations of this scenario
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            // Create and open the file picker
            FileOpenPicker openPicker = new FileOpenPicker();
            openPicker.ViewMode = PickerViewMode.Thumbnail;
            openPicker.SuggestedStartLocation = PickerLocationId.VideosLibrary;
            openPicker.FileTypeFilter.Add(".mp4");
            openPicker.FileTypeFilter.Add(".mkv");
            openPicker.FileTypeFilter.Add(".avi");

            StorageFile file = await openPicker.PickSingleFileAsync();
            if (file != null)
            {
                rootPage.NotifyUser("Picked video: " + file.Name, NotifyType.StatusMessage);
                mediaPlayer.Source = MediaSource.CreateFromStorageFile(file);
                mediaPlayer.Play();
            }
            else
            {
                rootPage.NotifyUser("Operation cancelled.", NotifyType.ErrorMessage);
            }
        }
    }
}
