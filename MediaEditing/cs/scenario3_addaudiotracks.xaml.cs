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
using Windows.Media.Editing;
using Windows.Media.Core;
using SDKTemplate;

namespace MediaEditingSample
{
    public sealed partial class Scenario3_AddAudioTracks : Page
    {
        private MainPage rootPage;
        private MediaComposition composition;
        private StorageFile pickedFile;
        private MediaStreamSource mediaStreamSource;

        public Scenario3_AddAudioTracks()
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

        private async void ChooseVideo_Click(object sender, RoutedEventArgs e)
        {
            var picker = new Windows.Storage.Pickers.FileOpenPicker();
            picker.SuggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.VideosLibrary;
            picker.FileTypeFilter.Add(".mp4");
            pickedFile = await picker.PickSingleFileAsync();
            if (pickedFile == null)
            {
                rootPage.NotifyUser("File picking cancelled", NotifyType.ErrorMessage);
                return;
            }

            mediaElement.SetSource(await pickedFile.OpenReadAsync(), pickedFile.ContentType);
            addAudio.IsEnabled = true;

            rootPage.NotifyUser("Video file added", NotifyType.StatusMessage);
        }

        private async void AddAudio_Click(object sender, RoutedEventArgs e)
        {
            // Create the original MediaComposition
            var clip = await MediaClip.CreateFromFileAsync(pickedFile);
            composition = new MediaComposition();
            composition.Clips.Add(clip);

            // Add background audio
            var picker = new Windows.Storage.Pickers.FileOpenPicker();
            picker.SuggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.MusicLibrary;
            picker.FileTypeFilter.Add(".mp3");
            picker.FileTypeFilter.Add(".wav");
            picker.FileTypeFilter.Add(".flac");
            var audioFile = await picker.PickSingleFileAsync();
            if (audioFile == null)
            {
                rootPage.NotifyUser("File picking cancelled", NotifyType.ErrorMessage);
                return;
            }

            var backgroundTrack = await BackgroundAudioTrack.CreateFromFileAsync(audioFile);
            composition.BackgroundAudioTracks.Add(backgroundTrack);

            // Render to MediaElement
            mediaElement.Position = TimeSpan.Zero;
            mediaStreamSource = composition.GeneratePreviewMediaStreamSource((int)mediaElement.ActualWidth, (int)mediaElement.ActualHeight);
            mediaElement.SetMediaStreamSource(mediaStreamSource);

            rootPage.NotifyUser("Background audio added", NotifyType.StatusMessage);
        }
    }
}
