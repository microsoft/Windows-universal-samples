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
using SDKTemplate;

using System;
using Windows.Media.Streaming.Adaptive;
using Windows.UI.Core;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AdaptiveStreaming
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2 : Page
    {
        private MainPage rootPage;
        AdaptiveMediaSource ams = null; //ams represents the AdaptiveMedaSource used throughout this sample

        public Scenario2()
        {
            this.InitializeComponent();
        }
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private void log(string s)
        {
            TextBlock text = new TextBlock();
            text.Text = s;
            text.TextWrapping = TextWrapping.WrapWholeWords;
            stkOutput.Children.Add(text);
        }

        private void btnPlay_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (string.IsNullOrEmpty(txtInputURL.Text))
            {
                rootPage.NotifyUser("Specify a URI to play", NotifyType.ErrorMessage);
                return;
            }

            InitializeAdaptiveMediaSource(new System.Uri(txtInputURL.Text), mePlayer);
        }

        async private void InitializeAdaptiveMediaSource(System.Uri uri, MediaElement m)
        {
            AdaptiveMediaSourceCreationResult result = await AdaptiveMediaSource.CreateFromUriAsync(uri);
            if(result.Status == AdaptiveMediaSourceCreationStatus.Success)
            {
                ams = result.MediaSource;
                m.SetMediaStreamSource(ams);

                outputBitrates(); //query for available bitrates and output to the log
                txtDownloadBitrate.Text = ams.InitialBitrate.ToString();
                txtPlaybackBitrate.Text = ams.InitialBitrate.ToString();

                //Register for download requests
                ams.DownloadRequested += DownloadRequested;

                //Register for bitrate change events
                ams.DownloadBitrateChanged += DownloadBitrateChanged;
                ams.PlaybackBitrateChanged += PlaybackBitrateChanged;
            }
            else
            {
                rootPage.NotifyUser("Error creating the AdaptiveMediaSource\n\t" + result.Status, NotifyType.ErrorMessage);
            }
        }

        private void DownloadRequested(AdaptiveMediaSource sender, AdaptiveMediaSourceDownloadRequestedEventArgs args)
        {
            UpdateBitrateUI(txtMeasuredBandwidth, (uint)ams.InboundBitsPerSecond);
        }

        private void DownloadBitrateChanged(AdaptiveMediaSource sender, AdaptiveMediaSourceDownloadBitrateChangedEventArgs args)
        {
            UpdateBitrateUI(txtDownloadBitrate, args.NewValue);
        }

        private void PlaybackBitrateChanged(AdaptiveMediaSource sender, AdaptiveMediaSourcePlaybackBitrateChangedEventArgs args)
        {
            UpdateBitrateUI(txtPlaybackBitrate, args.NewValue);
        }

        private async void UpdateBitrateUI(TextBlock t, uint newBitrate)
        {
            await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, new DispatchedHandler(() =>
             {
                 t.Text = newBitrate.ToString();
             }));
        }

        private void outputBitrates()
        {
            if (ams != null)
            {
                string bitrates = "Available bitrates: ";
                foreach (var b in ams.AvailableBitrates)
                {
                    bitrates += b + " ";
                }
                log(bitrates);
            }
            else
                rootPage.NotifyUser("Error: Adaptive Media Source is not initialized.", NotifyType.ErrorMessage);
        }

        private void btnSetInitialBitrate_Click(object sender, RoutedEventArgs e)
        {
            if(ams != null)
            {
                uint bitrate;
                if (uint.TryParse(txtInitialBitrate.Text, out bitrate))
                    try {
                        ams.InitialBitrate = bitrate;
                        log("Initial bitrate set to " + bitrate);
                    } catch (Exception)
                    {
                        log("Initial bitrate must match a value from the manifest");
                    }
                else
                    log("Initial bitrate must be set to a positive integer");
            }
            else
                rootPage.NotifyUser("Error: Adaptive Media Source is not initialized.", NotifyType.ErrorMessage);
        }

        private void btnSetMinBitrate_Click(object sender, RoutedEventArgs e)
        {
            if (ams != null)
            {
                uint bitrate;
                if (uint.TryParse(txtMinBitrate.Text, out bitrate))
                {
                    uint minbitrate = ams.AvailableBitrates[0];
                    foreach (var b in ams.AvailableBitrates) //iterate through the list of bitrates until the min valid bitrate is found
                    {
                        if(b >= bitrate)
                        {
                            minbitrate = b;
                            break;
                        }
                    }
                    ams.DesiredMinBitrate = minbitrate;
                    log("Min bitrate set to " + minbitrate);
                }
                else
                    log("Min bitrate must be a positive integer");
            }
            else
                rootPage.NotifyUser("Error: Adaptive Media Source is not initialized.", NotifyType.ErrorMessage);
        }

        private void btnSetMaxBitrate_Click(object sender, RoutedEventArgs e)
        {
            if (ams != null)
            {
                uint bitrate;
                if (uint.TryParse(txtMaxBitrate.Text, out bitrate))
                {
                    uint maxbitrate = ams.AvailableBitrates[ams.AvailableBitrates.Count-1]; //start with the max bitrate and work our way down
                    foreach (var b in ams.AvailableBitrates) //iterate through the list of bitrates until the max valid bitrate is found
                    {
                        if (b <= bitrate)
                            maxbitrate = b;
                        else
                            break;
                    }
                    ams.DesiredMaxBitrate = maxbitrate;
                    log("Max bitrate set to " + maxbitrate);
                }
                else
                    log("Max bitrate must be a positive integer");
            }
            else
                rootPage.NotifyUser("Error: Adaptive Media Source is not initialized.", NotifyType.ErrorMessage);
        }

        private void btnSetBandwidthMeasurementWindow_Click(object sender, RoutedEventArgs e)
        {
            if (ams != null)
            {
                int windowSize;
                if (int.TryParse(txtBandwidthMeasurementWindow.Text, out windowSize))
                {
                    ams.InboundBitsPerSecondWindow = new TimeSpan(0, 0, windowSize);
                    log("Bandwidth measurement window size set to " + windowSize);
                }
                else
                    log("Bandwidth measurement window size must be set to an int");
            }
            else
                rootPage.NotifyUser("Error: Adaptive Media Source is not initialized.", NotifyType.ErrorMessage);
        }
    }
}
