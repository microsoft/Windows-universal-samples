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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Storage;
using Windows.Foundation;
using Windows.Media.Editing;
using Windows.Media.Core;
using SDKTemplate;

namespace MediaEditingSample
{
    public sealed partial class Scenario4_AddOverlays : Page
    {
        private MainPage rootPage;
        private MediaComposition composition;
        private StorageFile baseVideoFile;
        private StorageFile overlayVideoFile;
        private MediaStreamSource mediaStreamSource;

        public Scenario4_AddOverlays()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            mediaElement.Source = null;
            mediaStreamSource = null;
            base.OnNavigatedFrom(e);
        }

        private async void ChooseBaseVideo_Click(object sender, RoutedEventArgs e)
        {
            var picker = new Windows.Storage.Pickers.FileOpenPicker();
            picker.SuggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.VideosLibrary;
            picker.FileTypeFilter.Add(".mp4");
            baseVideoFile = await picker.PickSingleFileAsync();
            if (baseVideoFile == null)
            {
                rootPage.NotifyUser("File picking cancelled", NotifyType.ErrorMessage);
                return;
            }

            mediaElement.SetSource(await baseVideoFile.OpenReadAsync(), baseVideoFile.ContentType);
            chooseOverlayVideo.IsEnabled = true;
        }

        private async void ChooseOverlayVideo_Click(object sender, RoutedEventArgs e)
        {
            var picker = new Windows.Storage.Pickers.FileOpenPicker();
            picker.SuggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.VideosLibrary;
            picker.FileTypeFilter.Add(".mp4");
            overlayVideoFile = await picker.PickSingleFileAsync();
            if (overlayVideoFile == null)
            {
                rootPage.NotifyUser("File picking cancelled", NotifyType.ErrorMessage);
                return;
            }

            CreateOverlays();
            rootPage.NotifyUser("Overlay video chosen", NotifyType.StatusMessage);
        }

        private async void CreateOverlays()
        {
            var baseVideoClip = await MediaClip.CreateFromFileAsync(baseVideoFile);
            composition = new MediaComposition();
            composition.Clips.Add(baseVideoClip);

            var overlayVideoClip = await MediaClip.CreateFromFileAsync(overlayVideoFile);

            // Overlay video in upper left corner, retain its native aspect ratio
            Rect videoOverlayPosition;
            var encodingProperties = overlayVideoClip.GetVideoEncodingProperties();
            videoOverlayPosition.Height = mediaElement.ActualHeight / 3;
            videoOverlayPosition.Width = (double)encodingProperties.Width / (double)encodingProperties.Height * videoOverlayPosition.Height;
            videoOverlayPosition.X = 0;
            videoOverlayPosition.Y = 0;

            var videoOverlay = new MediaOverlay(overlayVideoClip);
            videoOverlay.Position = videoOverlayPosition;
            videoOverlay.Opacity = 0.75;

            var overlayLayer = new MediaOverlayLayer();
            overlayLayer.Overlays.Add(videoOverlay);
            composition.OverlayLayers.Add(overlayLayer);

            // Render to MediaElement
            mediaElement.Position = TimeSpan.Zero;
            mediaStreamSource = composition.GeneratePreviewMediaStreamSource((int)mediaElement.ActualWidth, (int)mediaElement.ActualHeight);
            mediaElement.SetMediaStreamSource(mediaStreamSource);
            rootPage.NotifyUser("Overlays created", NotifyType.StatusMessage);
        }
    }
}