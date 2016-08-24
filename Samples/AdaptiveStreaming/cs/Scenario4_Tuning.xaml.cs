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
using System.Collections.Generic;
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
    ///
    /// Note: We register but do not unregister event handlers in this scenario, see the EventHandler
    ///       scenario for patterns that can be used to clean up.
    public sealed partial class Scenario4_Tuning : Page
    {
        public Scenario4_Tuning()
        {
            this.InitializeComponent();
            iconInboundBitsPerSecond.Symbol = (Symbol)0xE88A;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            ctsForInboundBitsPerSecondUiRefresh.Cancel(); // Cancel timer
            adaptiveMS = null; // release app instance of AdaptiveMediaSource
            mpItem = null; // release app instance of MediaPlaybackItem
            var mp = mediaPlayerElement.MediaPlayer;
            if (mp != null)
            {
                mp.DisposeSource();
                mediaPlayerElement.SetMediaPlayer(null);
                mp.Dispose();
            }
        }

        CancellationTokenSource ctsForInboundBitsPerSecondUiRefresh = new CancellationTokenSource();
        private AdaptiveContentModel adaptiveContentModel;
        private AdaptiveMediaSource adaptiveMS;
        private MediaPlaybackItem mpItem;
        private BitrateHelper bitrateHelper;

        private async void Page_OnLoaded(object sender, RoutedEventArgs e)
        {
            var mediaPlayer = new MediaPlayer();
            RegisterForMediaPlayerEvents(mediaPlayer);

            // Ensure we have PlayReady support, if the user enters a DASH/PR Uri in the text box:
            var prHelper = new PlayReadyHelper(LoggerControl);
            prHelper.SetUpProtectionManager(mediaPlayer);
            mediaPlayerElement.SetMediaPlayer(mediaPlayer);

            // Choose a default content.
            SelectedContent.ItemsSource = MainPage.ContentManagementSystemStub.ToArray();
            adaptiveContentModel = MainPage.FindContentById(10);
            SelectedContent.SelectedItem = adaptiveContentModel;

            UriBox.Text = adaptiveContentModel.ManifestUri.ToString();
            await LoadSourceFromUriAsync(adaptiveContentModel.ManifestUri);

            // There is no InboundBitsPerSecondChanged event, so we start a polling thread to update UI.
            PollForInboundBitsPerSecond(ctsForInboundBitsPerSecondUiRefresh);
        }

        private async void PollForInboundBitsPerSecond(CancellationTokenSource cts)
        {
            ulong InboundBitsPerSecondLast = 0;
            var refreshRate = TimeSpan.FromSeconds(2);
            while (!cts.IsCancellationRequested)
            {
                if (adaptiveMS != null)
                {
                    if (InboundBitsPerSecondLast != adaptiveMS.InboundBitsPerSecond)
                    {
                        InboundBitsPerSecondLast = adaptiveMS.InboundBitsPerSecond;
                        InboundBitsPerSecondText.Text = InboundBitsPerSecondLast.ToString();
                    }
                }
                await Task.Delay(refreshRate);
            }
        }


        #region Content Loading

        private void HideDescriptionOnSmallScreen()
        {
            // On small screens, hide the description text to make room for the video.
            DescriptionText.Visibility = (ActualHeight < 700) ? Visibility.Collapsed : Visibility.Visible;
        }

        private async void LoadId_Click(object sender, RoutedEventArgs e)
        {
            adaptiveContentModel = (AdaptiveContentModel)SelectedContent.SelectedItem;

            UriBox.Text = adaptiveContentModel.ManifestUri.ToString();
            await LoadSourceFromUriAsync(adaptiveContentModel.ManifestUri);
        }

        private async void LoadUri_Click(object sender, RoutedEventArgs e)
        {
            Uri uri;
            if (!Uri.TryCreate(UriBox.Text, UriKind.Absolute, out uri))
            {
                Log("Malformed Uri in Load text box.");
            }

            await LoadSourceFromUriAsync(uri);

            HideDescriptionOnSmallScreen();
        }

        private void SetSource_Click(object sender, RoutedEventArgs e)
        {
            // It is at this point that the MediaSource (within a MediaPlaybackItem) will be fully resolved.
            // For an AdaptiveMediaSource, this is the point at which media is first requested and parsed.
            mediaPlayerElement.Source = mpItem;

            HideDescriptionOnSmallScreen();
        }

        private async Task LoadSourceFromUriAsync(Uri uri, HttpClient httpClient = null)
        {
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
            if (result.Status == AdaptiveMediaSourceCreationStatus.Success)
            {
                adaptiveMS = result.MediaSource;

                bitrateHelper = new BitrateHelper(adaptiveMS.AvailableBitrates);
                InitializeBitrateLists(adaptiveMS);

                // At this point, we have read the manifest of the media source, and all bitrates are known.
                await UpdatePlaybackBitrate(adaptiveMS.CurrentPlaybackBitrate);
                await UpdateDownloadBitrate(adaptiveMS.CurrentDownloadBitrate);

                // Register for events before resolving the MediaSource.
                RegisterForAdaptiveMediaSourceEvents(adaptiveMS);

                MediaSource source = MediaSource.CreateFromAdaptiveMediaSource(adaptiveMS);

                // You can save additional information in the CustomPropertySet for future retrieval.
                // Note: MediaSource.CustomProperties is a ValueSet and therefore can store
                // only serializable types.

                // Save the original Uri.
                source.CustomProperties.Add("uri", uri.ToString());

                // You're likely to put a content tracking id into the CustomProperties.
                source.CustomProperties.Add("contentId", Guid.NewGuid());

                mpItem = new MediaPlaybackItem(source);
            }
            else
            {
                Log("Error creating the AdaptiveMediaSource: " + result.Status);
            }
        }

        #endregion

        #region MediaPlayer Event Handlers
        private void RegisterForMediaPlayerEvents(MediaPlayer mediaPlayer)
        {
            mediaPlayer.PlaybackSession.NaturalVideoSizeChanged +=
                (sender, args) => Log("PlaybackSession.NaturalVideoSizeChanged, NaturalVideoWidth: " + sender.NaturalVideoWidth + " NaturalVideoHeight: " + sender.NaturalVideoHeight);
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
            if (selection != null && adaptiveMS.InitialBitrate != selection.Bitrate)
            {
                if (IsValidBitrateCombination(adaptiveMS.DesiredMinBitrate, adaptiveMS.DesiredMaxBitrate, selection.Bitrate.Value))
                {
                    adaptiveMS.InitialBitrate = selection.Bitrate.Value;
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
            if (selection != null && adaptiveMS.DesiredMinBitrate != selection.Bitrate)
            {
                if (IsValidBitrateCombination(selection.Bitrate, adaptiveMS.DesiredMaxBitrate, adaptiveMS.InitialBitrate))
                {
                    adaptiveMS.DesiredMinBitrate = selection.Bitrate;
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
            if (selection != null && adaptiveMS.DesiredMaxBitrate != selection.Bitrate)
            {
                if (IsValidBitrateCombination(adaptiveMS.DesiredMinBitrate, selection.Bitrate, adaptiveMS.InitialBitrate))
                {
                    adaptiveMS.DesiredMaxBitrate = selection.Bitrate;
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
                adaptiveMS.AdvancedSettings.BitrateDowngradeTriggerRatio = ratio;
            }
        }

        private void SetDesiredBitrateHeadroomRatio_Click()
        {
            double ratio;
            if (double.TryParse(BitrateDowngradeTriggerRatioText.Text, out ratio))
            {
                adaptiveMS.AdvancedSettings.DesiredBitrateHeadroomRatio = ratio;
            }
        }
        #endregion

        #region AdaptiveMediaSource Event Handlers

        private void RegisterForAdaptiveMediaSourceEvents(AdaptiveMediaSource aMS)
        {
            aMS.DownloadFailed += DownloadFailed;
            aMS.DownloadBitrateChanged += DownloadBitrateChanged;
            aMS.PlaybackBitrateChanged += PlaybackBitrateChanged;
        }

        private void DownloadFailed(AdaptiveMediaSource sender, AdaptiveMediaSourceDownloadFailedEventArgs args)
        {
            Log($"DownloadFailed: {args.HttpResponseMessage}, {args.ResourceType}, {args.ResourceUri}");
        }

        private async void DownloadBitrateChanged(AdaptiveMediaSource sender, AdaptiveMediaSourceDownloadBitrateChangedEventArgs args)
        {
            uint downloadBitrate = args.NewValue;
            await UpdateDownloadBitrate(downloadBitrate);
        }

        private async Task UpdateDownloadBitrate(uint downloadBitrate)
        {
            Log("DownloadBitrateChanged: " + downloadBitrate);
            await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, new DispatchedHandler(() =>
            {
                iconDownloadBitrate.Symbol = bitrateHelper.GetBitrateSymbol(downloadBitrate);
                txtDownloadBitrate.Text = downloadBitrate.ToString();
            }));
        }

        private async void PlaybackBitrateChanged(AdaptiveMediaSource sender, AdaptiveMediaSourcePlaybackBitrateChangedEventArgs args)
        {
            uint playbackBitrate = args.NewValue;
            await UpdatePlaybackBitrate(playbackBitrate);
        }

        private async Task UpdatePlaybackBitrate(uint playbackBitrate)
        {
            Log("PlaybackBitrateChanged: " + playbackBitrate);
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
