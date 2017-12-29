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
using SDKTemplate.Logging;
using System;
using System.Linq;
using System.Threading.Tasks;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.Media.Streaming.Adaptive;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Web.Http;
using Windows.Web.Http.Filters;

namespace SDKTemplate
{
    /// See the README.md for discussion of this scenario.
    public sealed partial class Scenario2_EventHandlers : Page
    {
        private BitrateHelper bitrateHelper;

        public Scenario2_EventHandlers()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            // Release handles on various media objects to ensure a quick clean-up
            ContentSelectorControl.MediaPlaybackItem = null;
            var mediaPlayer = mediaPlayerElement.MediaPlayer;
            if (mediaPlayer != null)
            {
                mediaPlayerLogger?.Dispose();
                mediaPlayerLogger = null;

                UnregisterHandlers(mediaPlayer);

                mediaPlayer.DisposeSource();
                mediaPlayerElement.SetMediaPlayer(null);
                mediaPlayer.Dispose();
            }
        }

        private void UnregisterHandlers(MediaPlayer mediaPlayer)
        {
            AdaptiveMediaSource adaptiveMediaSource = null;
            MediaPlaybackItem mpItem = mediaPlayer.Source as MediaPlaybackItem;
            if (mpItem != null)
            {
                adaptiveMediaSource = mpItem.Source.AdaptiveMediaSource;
            }
            MediaSource source = mediaPlayer.Source as MediaSource;
            if (source != null)
            {
                adaptiveMediaSource = source.AdaptiveMediaSource;
            }

            mediaPlaybackItemLogger?.Dispose();
            mediaPlaybackItemLogger = null;

            mediaSourceLogger?.Dispose();
            mediaSourceLogger = null;

            adaptiveMediaSourceLogger?.Dispose();
            adaptiveMediaSourceLogger = null;

            UnregisterForAdaptiveMediaSourceEvents(adaptiveMediaSource);
        }

        private void Page_OnLoaded(object sender, RoutedEventArgs e)
        {
            // Explicitly create the instance of MediaPlayer if you need to register for its events
            // (like MediaOpened / MediaFailed) prior to setting an IMediaPlaybackSource.
            var mediaPlayer = new MediaPlayer();

            // We use a helper class that logs all the events for the MediaPlayer:
            mediaPlayerLogger = new MediaPlayerLogger(LoggerControl, mediaPlayer);

            // Ensure we have PlayReady support, in case the user enters a DASH/PR Uri in the text box.
            var prHelper = new PlayReadyHelper(LoggerControl);
            prHelper.SetUpProtectionManager(mediaPlayer);
            mediaPlayerElement.SetMediaPlayer(mediaPlayer);

            // This Scenario focuses on event handling, mostly via the "Logger" helper classes
            // in the Shared folder.
            //
            // In addition, the App can also insert an IHttpFilter into the Windows.Web.Http stack
            // which is used by the AdaptiveMediaSource.  To do so, it must derive a class from
            // IHttpFilter, provide the HttpBaseProtocolFilter to that class to execute requests,
            // then use the derived filter as the constructor for the HttpClient.
            //
            // The HttpClient is then used in the constructor of the AdaptiveMediaSource.
            // The App should explicitly set:
            //   CacheControl.WriteBehavior = HttpCacheWriteBehavior.NoCache
            // When passing an HttpClient into the constructor of AdpativeMediaSource,
            // set this on the HttpBaseProtocolFilter.
            //
            // More than one IHttpFilter can be nested, for example, an App might
            // use a filter to modify web requests: see Scenario3 for details.
            //
            // In this scenario, we add an AdaptiveMediaSourceHttpFilterLogger to provide additional
            // verbose logging details for each request.
            var baseProtocolFilter = new HttpBaseProtocolFilter();
            baseProtocolFilter.CacheControl.WriteBehavior = HttpCacheWriteBehavior.NoCache;  // Always set WriteBehavior = NoCache
            var fistLevelFilter = new AdaptiveMediaSourceHttpFilterLogger(LoggerControl, baseProtocolFilter);
            var httpClient = new HttpClient(fistLevelFilter);


            ContentSelectorControl.Initialize(
                mediaPlayer,
                MainPage.ContentManagementSystemStub.Where(m => !m.Aes),
                httpClient,
                LoggerControl,
                LoadSourceFromUriAsync);
        }


        #region Content Loading

        private async Task<MediaPlaybackItem> LoadSourceFromUriAsync(Uri uri, HttpClient httpClient = null)
        {
            UnregisterHandlers(mediaPlayerElement.MediaPlayer);
            mediaPlayerElement.MediaPlayer?.DisposeSource();

            AdaptiveMediaSourceCreationResult result = null;
            if (httpClient != null)
            {
                result = await AdaptiveMediaSource.CreateFromUriAsync(uri, httpClient);
            }
            else
            {
                result = await AdaptiveMediaSource.CreateFromUriAsync(uri);
            }

            MediaSource source;
            if (result.Status == AdaptiveMediaSourceCreationStatus.Success)
            {
                var adaptiveMediaSource = result.MediaSource;

                // We use a helper class that logs all the events for the AdaptiveMediaSource:
                adaptiveMediaSourceLogger = new AdaptiveMediaSourceLogger(LoggerControl, adaptiveMediaSource);

                // In addition to logging, we use the callbacks to update some UI elements in this scenario:
                RegisterForAdaptiveMediaSourceEvents(adaptiveMediaSource);

                // At this point, we have read the manifest of the media source, and all bitrates are known.
                bitrateHelper = new BitrateHelper(adaptiveMediaSource.AvailableBitrates);

                // The AdaptiveMediaSource chooses initial playback and download bitrates.
                // See the Tuning scenario for examples of customizing these bitrates.
                await UpdatePlaybackBitrateAsync(adaptiveMediaSource.CurrentPlaybackBitrate);
                await UpdateDownloadBitrateAsync(adaptiveMediaSource.CurrentDownloadBitrate);

                source = MediaSource.CreateFromAdaptiveMediaSource(adaptiveMediaSource);

            }
            else
            {
                Log($"Error creating the AdaptiveMediaSource. Status: {result.Status}, ExtendedError.Message: {result.ExtendedError.Message}, ExtendedError.HResult: {result.ExtendedError.HResult.ToString("X8")}");

                // In this scenario, we make a second attempt to load any URI that is
                // not adaptive streaming as a progressive download URI:
                Log($"Attempting to create a MediaSource from uri: {uri}");
                source = MediaSource.CreateFromUri(uri);
            }

            // We use a helper class that logs all the events for the MediaSource:
            mediaSourceLogger = new MediaSourceLogger(LoggerControl, source);

            // You can save additional information in the CustomPropertySet for future retrieval.
            // Note: MediaSource.CustomProperties is a ValueSet and therefore can store
            // only serializable types.

            // Save the original Uri.
            source.CustomProperties["uri"] = uri.ToString();

            // You're likely to put a content tracking id into the CustomProperties.
            source.CustomProperties["contentId"] = Guid.NewGuid().ToString();

            var mpItem = new MediaPlaybackItem(source);

            // We use a helper class that logs all the events for the MediaPlaybackItem:
            mediaPlaybackItemLogger = new MediaPlaybackItemLogger(LoggerControl, mpItem);

            HideDescriptionOnSmallScreen();

            return mpItem;
        }

        private async void HideDescriptionOnSmallScreen()
        {
            // On small screens, hide the description text to make room for the video.
            await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                DescriptionText.Visibility = (ActualHeight < 500) ? Visibility.Collapsed : Visibility.Visible;
            });
        }
        #endregion


        #region AdaptiveMediaSource Event Handlers

        private void RegisterForAdaptiveMediaSourceEvents(AdaptiveMediaSource adaptiveMediaSource)
        {
            adaptiveMediaSource.DownloadBitrateChanged += DownloadBitrateChanged;
            adaptiveMediaSource.PlaybackBitrateChanged += PlaybackBitrateChanged;
        }

        private void UnregisterForAdaptiveMediaSourceEvents(AdaptiveMediaSource adaptiveMediaSource)
        {
            if (adaptiveMediaSource != null)
            {
                adaptiveMediaSource.DownloadBitrateChanged -= DownloadBitrateChanged;
                adaptiveMediaSource.PlaybackBitrateChanged -= PlaybackBitrateChanged;
            }
        }
        private async void DownloadBitrateChanged(AdaptiveMediaSource sender, AdaptiveMediaSourceDownloadBitrateChangedEventArgs args)
        {
            uint downloadBitrate = args.NewValue;
            await UpdateDownloadBitrateAsync(downloadBitrate);
        }

        private async Task UpdateDownloadBitrateAsync(uint downloadBitrate)
        {
            await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, new DispatchedHandler(() =>
            {
                iconDownloadBitrate.Symbol = bitrateHelper.GetBitrateSymbol(downloadBitrate);
                txtDownloadBitrate.Text = downloadBitrate.ToString();
            }));
        }

        private async void PlaybackBitrateChanged(AdaptiveMediaSource sender, AdaptiveMediaSourcePlaybackBitrateChangedEventArgs args)
        {
            uint playbackBitrate = args.NewValue;
            await UpdatePlaybackBitrateAsync(playbackBitrate);
        }

        private async Task UpdatePlaybackBitrateAsync(uint playbackBitrate)
        {
            await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, new DispatchedHandler(() =>
            {
                iconPlaybackBitrate.Symbol = bitrateHelper.GetBitrateSymbol(playbackBitrate);
                txtPlaybackBitrate.Text = playbackBitrate.ToString();
            }));
        }
        #endregion


        #region Utilities
        private void Log(string message)
        {
            LoggerControl.Log(message);
        }
        MediaPlayerLogger mediaPlayerLogger;
        MediaSourceLogger mediaSourceLogger;
        MediaPlaybackItemLogger mediaPlaybackItemLogger;
        AdaptiveMediaSourceLogger adaptiveMediaSourceLogger;
        #endregion
    }
}

