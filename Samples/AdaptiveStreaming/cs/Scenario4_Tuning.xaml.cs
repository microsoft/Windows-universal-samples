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
using System.Threading;
using System.Threading.Tasks;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.Media.Streaming.Adaptive;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Web.Http;

namespace SDKTemplate
{
    /// See the README.md for discussion of this scenario.

    public sealed partial class Scenario4_Tuning : Page
    {
        CancellationTokenSource ctsForInboundBitsPerSecondUiRefresh = new CancellationTokenSource();
        private AdaptiveMediaSource adaptiveMediaSource;
        private BitrateHelper bitrateHelper;

        public Scenario4_Tuning()
        {
            this.InitializeComponent();
            iconInboundBitsPerSecond.Symbol = (Symbol)0xE88A;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            ctsForInboundBitsPerSecondUiRefresh.Cancel(); // Cancel timer
            adaptiveMediaSource = null; // release app instance of AdaptiveMediaSource

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
            var mediaPlayer = new MediaPlayer();

            // We use a helper class that logs all the events for the MediaPlayer:
            mediaPlayerLogger = new MediaPlayerLogger(LoggerControl, mediaPlayer);

            // Ensure we have PlayReady support, if the user enters a DASH/PR Uri in the text box:
            var prHelper = new PlayReadyHelper(LoggerControl);
            prHelper.SetUpProtectionManager(mediaPlayer);
            mediaPlayerElement.SetMediaPlayer(mediaPlayer);

            ContentSelectorControl.Initialize(
                mediaPlayer,
                MainPage.ContentManagementSystemStub.Where(m => !m.Aes),
                null,
                LoggerControl,
                LoadSourceFromUriAsync);

            // There is no InboundBitsPerSecondChanged event, so we start a polling thread to update UI.
            PollForInboundBitsPerSecond(ctsForInboundBitsPerSecondUiRefresh);
        }

        private async void PollForInboundBitsPerSecond(CancellationTokenSource cts)
        {
            ulong InboundBitsPerSecondLast = 0;
            var refreshRate = TimeSpan.FromSeconds(2);
            while (!cts.IsCancellationRequested)
            {
                if (adaptiveMediaSource != null)
                {
                    if (InboundBitsPerSecondLast != adaptiveMediaSource.InboundBitsPerSecond)
                    {
                        InboundBitsPerSecondLast = adaptiveMediaSource.InboundBitsPerSecond;
                        InboundBitsPerSecondText.Text = InboundBitsPerSecondLast.ToString();
                    }
                }
                await Task.Delay(refreshRate);
            }
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
                adaptiveMediaSource = result.MediaSource;

                // We use a helper class that logs all the events for the AdaptiveMediaSource:
                adaptiveMediaSourceLogger = new AdaptiveMediaSourceLogger(LoggerControl, adaptiveMediaSource);

                // In addition to logging, we use the callbacks to update some UI elements in this scenario:
                RegisterForAdaptiveMediaSourceEvents(adaptiveMediaSource);

                // At this point, we have read the manifest of the media source, and all bitrates are known.
                bitrateHelper = new BitrateHelper(adaptiveMediaSource.AvailableBitrates);
                InitializeBitrateLists(adaptiveMediaSource);
                await UpdatePlaybackBitrateAsync(adaptiveMediaSource.CurrentPlaybackBitrate);
                await UpdateDownloadBitrateAsync(adaptiveMediaSource.CurrentDownloadBitrate);

                source = MediaSource.CreateFromAdaptiveMediaSource(adaptiveMediaSource);
            }
            else
            {
                Log($"Error creating the AdaptiveMediaSource. Status: {result.Status}, ExtendedError.Message: {result.ExtendedError.Message}, ExtendedError.HResult: {result.ExtendedError.HResult.ToString("X8")}");
                return null;
            }

            // We use a helper class that logs all the events for the MediaSource:
            mediaSourceLogger = new MediaSourceLogger(LoggerControl, source);

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


        #region Adaptive Bitrate control

        private void InitializeBitrateLists(AdaptiveMediaSource aMS)
        {
            var sortedBitrates = aMS.AvailableBitrates.OrderByDescending(br => br).Select(br => new BitrateItem(br)).ToArray();
            InitialBitrateList.ItemsSource = sortedBitrates;
            var selected = sortedBitrates.First(item => item.Bitrate == aMS.InitialBitrate);
            InitialBitrateList.SelectedItem = sortedBitrates.FirstOrDefault(item => item.Bitrate == aMS.InitialBitrate);

            var nullableSortedBitrates = (new BitrateItem[] { new BitrateItem(null) }).Concat(sortedBitrates).ToArray();
            DesiredMaxBitrateList.ItemsSource = DesiredMinBitrateList.ItemsSource = nullableSortedBitrates;

            DesiredMaxBitrateList.SelectedItem = nullableSortedBitrates.First(item => item.Bitrate == aMS.DesiredMaxBitrate);
            DesiredMinBitrateList.SelectedItem = nullableSortedBitrates.First(item => item.Bitrate == aMS.DesiredMinBitrate);
        }

        // An argument exception will be thrown if the following constraint is not met:
        //   DesiredMinBitrate <= InitialBitrate <= DesiredMaxBitrate
        private bool IsValidBitrateCombination(uint? desiredMinBitrate, uint? desiredMaxBitrate, uint initialBitrate)
        {
            // The ">" operator returns false if either operand is null. We take advantage of this
            // by testing in this manner. Do NOT "optimize" this by reversing the sense of the test
            // to "return desiredMinBitrate <= initialBitrate && initialBitrate <= desiredMaxBitrate;"
            if (desiredMinBitrate > initialBitrate || initialBitrate > desiredMaxBitrate)
            {
                return false;
            }
            return true;
        }

        private void InitialBitrateList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var selection = (e.AddedItems.Count > 0) ? (BitrateItem)e.AddedItems[0] : null;
            if (selection != null && adaptiveMediaSource.InitialBitrate != selection.Bitrate)
            {
                if (IsValidBitrateCombination(adaptiveMediaSource.DesiredMinBitrate, adaptiveMediaSource.DesiredMaxBitrate, selection.Bitrate.Value))
                {
                    adaptiveMediaSource.InitialBitrate = selection.Bitrate.Value;
                }
                else
                {
                    InitialBitrateList.SelectedItem = e.RemovedItems[0];
                }
            }
        }

        private void DesiredMinBitrateList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var selection = (e.AddedItems.Count > 0) ? (BitrateItem)e.AddedItems[0] : null;
            if (selection != null && adaptiveMediaSource.DesiredMinBitrate != selection.Bitrate)
            {
                if (IsValidBitrateCombination(selection.Bitrate, adaptiveMediaSource.DesiredMaxBitrate, adaptiveMediaSource.InitialBitrate))
                {
                    adaptiveMediaSource.DesiredMinBitrate = selection.Bitrate;
                }
                else
                {
                    DesiredMinBitrateList.SelectedItem = e.RemovedItems[0];
                }
            }
        }

        private void DesiredMaxBitrateList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var selection = (e.AddedItems.Count > 0) ? (BitrateItem)e.AddedItems[0] : null;
            if (selection != null && adaptiveMediaSource.DesiredMaxBitrate != selection.Bitrate)
            {
                if (IsValidBitrateCombination(adaptiveMediaSource.DesiredMinBitrate, selection.Bitrate, adaptiveMediaSource.InitialBitrate))
                {
                    adaptiveMediaSource.DesiredMaxBitrate = selection.Bitrate;
                }
                else
                {
                    DesiredMaxBitrateList.SelectedItem = e.RemovedItems[0];
                }
            }
        }

        private void SetBitrateDowngradeTriggerRatio_Click()
        {
            double ratio;
            if (double.TryParse(BitrateDowngradeTriggerRatioText.Text, out ratio))
            {
                adaptiveMediaSource.AdvancedSettings.BitrateDowngradeTriggerRatio = ratio;
            }
        }

        private void SetDesiredBitrateHeadroomRatio_Click()
        {
            double ratio;
            if (double.TryParse(BitrateDowngradeTriggerRatioText.Text, out ratio))
            {
                adaptiveMediaSource.AdvancedSettings.DesiredBitrateHeadroomRatio = ratio;
            }
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


    /// <summary>
    /// Item which provides a nicer display name for "null" bitrate.
    /// </summary>
    class BitrateItem
    {
        public uint? Bitrate { get; private set; }
        public BitrateItem(uint? bitrate)
        {
            Bitrate = bitrate;
        }

        public override string ToString()
        {
            return (Bitrate == null) ? "Not set" : Bitrate.ToString();
        }
    }
}
