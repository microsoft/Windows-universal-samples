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

using SDKTemplate.Models;
using SDKTemplate.Logging;
using System;
using System.Collections.Generic;
using System.Linq;
using Windows.ApplicationModel.DataTransfer;
using Windows.Media.Playback;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using System.Threading.Tasks;
using Windows.Web.Http;

namespace SDKTemplate.Controls
{
    public sealed partial class ContentSelector : UserControl
    {
        public ContentSelector()
        {
            this.InitializeComponent();
        }

        private MediaPlayer mediaPlayer;
        private IEnumerable<AdaptiveContentModel> adaptiveContentModels;
        public AdaptiveContentModel SelectedModel;
        private LogView loggerControl;
        Func<Uri, HttpClient, Task<MediaPlaybackItem>> CreateMediaPlaybackItem;
        public MediaPlaybackItem MediaPlaybackItem;
        public HttpClient optionalHttpClient;

        internal void Initialize(MediaPlayer mediaPlayer, 
            IEnumerable<AdaptiveContentModel> adaptiveContentModels,
            HttpClient optionalHttpClient,
            LogView loggerControl, 
            Func<Uri, HttpClient, Task<MediaPlaybackItem>> loadSourceFromUriAsync)
        {
            if (mediaPlayer == null)
                throw new ArgumentNullException("mediaPlayer");
            this.mediaPlayer = mediaPlayer;

            if (adaptiveContentModels == null)
                throw new ArgumentNullException("adaptiveContentModels");
            this.adaptiveContentModels = adaptiveContentModels;

            if (loggerControl == null)
                throw new ArgumentNullException("loggerControl");
            this.loggerControl = loggerControl;

            if (loadSourceFromUriAsync == null)
                throw new ArgumentNullException("loadSourceFromUriAsync");
            this.CreateMediaPlaybackItem = loadSourceFromUriAsync;

            this.optionalHttpClient = optionalHttpClient;

            // Set data context:
            SelectedContent.ItemsSource = this.adaptiveContentModels;
            // Set default item:
            SetSelectedModel(this.adaptiveContentModels.First());
        }

        public void SetSelectedModel(AdaptiveContentModel model)
        {
            SelectedModel = model;
            SelectedContent.SelectedItem = SelectedModel;
        }

        public async void HideLoadUri()
        {
            await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                this.LoadUriPanel.Visibility = Visibility.Collapsed;
            });
        }
        public async void SetAutoPlay(bool IsChecked)
        {
            await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                AutoPlay.IsChecked = IsChecked;
            });
        }

        private async void LoadId_Click(object sender, RoutedEventArgs e)
        {
            if (mediaPlayer == null || adaptiveContentModels == null || loggerControl == null)
                return;

            this.SelectedModel = (AdaptiveContentModel)SelectedContent.SelectedItem;

            UriBox.Text = SelectedModel.ManifestUri.ToString();
            await CreateNewMediaPlaybackItem(SelectedModel.ManifestUri);
        }

        private async void LoadUri_Click(object sender, RoutedEventArgs e)
        {
            if (mediaPlayer == null || adaptiveContentModels == null || loggerControl == null)
                return;

            Uri uri;
            if (!Uri.TryCreate(UriBox.Text, UriKind.Absolute, out uri))
            {
                loggerControl.Log("Malformed Uri in Load text box.", SDKTemplate.Logging.LogViewLoggingLevel.Critical);
                return;
            }
            await CreateNewMediaPlaybackItem(uri);
        }

        private async Task CreateNewMediaPlaybackItem(Uri uri)
        {
            SetSourceEnabled(false);
            this.MediaPlaybackItem = null;
            this.MediaPlaybackItem = await CreateMediaPlaybackItem(uri, optionalHttpClient);
            if (this.MediaPlaybackItem != null)
            {
                loggerControl.Log($"Loaded Uri: {uri}");
                SetSourceEnabled(true);
            }
        }

        private async void SetSourceEnabled(bool SetEnabled)
        {
            await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                SetSource.IsEnabled = SetEnabled;
            });
        }

        private void AutoPlay_Checked(object sender, RoutedEventArgs e)
        {
            if (mediaPlayer == null || adaptiveContentModels == null || loggerControl == null)
                return;

            mediaPlayer.AutoPlay = (bool)(sender as CheckBox).IsChecked;
        }

        private void ForceEquirectangularSphericalVideoFrameFormat()
        {
            if (mediaPlayer.PlaybackSession.PlaybackState == MediaPlaybackState.None ||
                mediaPlayer.PlaybackSession.PlaybackState == MediaPlaybackState.Opening)
                return;

            var mpsvp = mediaPlayer.PlaybackSession.SphericalVideoProjection;
            if (mpsvp == null)
                return;

            if (mpsvp.FrameFormat != Windows.Media.MediaProperties.SphericalVideoFrameFormat.Equirectangular)
                mpsvp.FrameFormat = Windows.Media.MediaProperties.SphericalVideoFrameFormat.Equirectangular;

            if (mpsvp.IsEnabled != true)
                mpsvp.IsEnabled = true;

            mpsvp.HorizontalFieldOfViewInDegrees = 120;
        }

        private void SetSource_Click(object sender, RoutedEventArgs e)
        {
            if (mediaPlayer == null || adaptiveContentModels == null || loggerControl == null)
                return;
            
            // It is at this point that the MediaSource (within a MediaPlaybackItem) will be fully resolved.
            // For an AdaptiveMediaSource, this is the point at which media is first requested and parsed.
            mediaPlayer.Source = this.MediaPlaybackItem;
        }
    }
}
