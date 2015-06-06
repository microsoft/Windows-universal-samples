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
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.UI.Core;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

namespace VideoPlayback
{
    /// <summary>
    /// Demonstrates playing video lists using MediaPlaybackList.
    /// </summary>
    public sealed partial class Scenario7 : Page
    {
        private MainPage rootPage;
        private Dictionary<string, BitmapImage> artCache = new Dictionary<string, BitmapImage>();
        private MediaPlaybackList playbackList = new MediaPlaybackList();

        public Scenario7()
        {
            this.InitializeComponent();

            // Always use the cached page
            this.NavigationCacheMode = NavigationCacheMode.Required;

            // Create a static playback list
            InitializePlaybackList();

            // Subscribe to MediaElement events
            mediaElement.CurrentStateChanged += MediaElement_CurrentStateChanged;

            // Subscribe to list UI changes
            playlistView.ItemClick += PlaylistView_ItemClick;
        }

        void InitializePlaybackList()
        {
            // Initialize the playlist data/view model.
            // In a production app your data would be sourced from a data store or service.

            // Add content
            var media1 = new MediaModel();
            media1.Title = "Fitness";
            media1.MediaUri = new Uri("ms-appx:///Assets/Media/multivideo-with-captions.mkv");
            media1.ArtUri = new Uri("ms-appx:///Assets/Media/multivideo.jpg");
            playlistView.Media.Add(media1);

            var media2 = new MediaModel();
            media2.Title = "Elephant's Dream";
            media2.MediaUri = new Uri("ms-appx:///Assets/Media/ElephantsDream-Clip-H264_SD-AAC_eng-AAC_spa-AAC_eng_commentary-SRT_eng-SRT_por-SRT_swe.mkv");
            media2.ArtUri = new Uri("ms-appx:///Assets/Media/ElephantsDream.jpg");
            playlistView.Media.Add(media2);

            var media3 = new MediaModel();
            media3.Title = "Sintel";
            media3.MediaUri = new Uri("ms-appx:///Assets/Media/sintel_trailer-480p.mp4");
            media3.ArtUri = new Uri("ms-appx:///Assets/Media/sintel.jpg");
            playlistView.Media.Add(media3);

            // Pre-cache all album art to facilitate smooth gapless transitions.
            // A production app would have a more sophisticated object cache.
            foreach (var media in playlistView.Media)
            {
                var bitmap = new BitmapImage();
                bitmap.UriSource = media.ArtUri;
                artCache[media.ArtUri.ToString()] = bitmap;
            }

            // Initialize the playback list for this content
            foreach(var media in playlistView.Media)
            {
                var mediaSource = MediaSource.CreateFromUri(media.MediaUri);
                mediaSource.CustomProperties["uri"] = media.MediaUri;

                var playbackItem = new MediaPlaybackItem(mediaSource);

                playbackList.Items.Add(playbackItem);
            }

            // Subscribe for changes
            playbackList.CurrentItemChanged += PlaybackList_CurrentItemChanged;

            // Loop
            playbackList.AutoRepeatEnabled = true;
        }

        private void PlaybackList_CurrentItemChanged(MediaPlaybackList sender, CurrentMediaPlaybackItemChangedEventArgs args)
        {
            var ignoreAwaitWarning = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                var currentItem = sender.CurrentItem;
                playlistView.SelectedIndex = playbackList.Items.ToList().FindIndex(i => i == currentItem);
            });
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.
        /// This parameter is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            // Set list for playback
            mediaElement.SetPlaybackSource(playbackList);
        }

        /// <summary>
        /// MediaPlayer state changed event handlers. 
        /// Note that we can subscribe to events even if Media Player is playing media in background
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private async void MediaElement_CurrentStateChanged(object sender, RoutedEventArgs e)
        {
            var currentState = mediaElement.CurrentState; // cache outside of completion or you might get a different value
            await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                // Update state label
                txtCurrentState.Text = currentState.ToString();

                // Update controls
                UpdateTransportControls(currentState);
            });
        }

        private void PlaylistView_ItemClick(object sender, ItemClickEventArgs e)
        {
            var item = e.ClickedItem as MediaModel;
            Debug.WriteLine("Clicked item: " + item.MediaUri.ToString());

            // Start the background task if it wasn't running
            playbackList.MoveTo((uint)playbackList.Items.ToList().FindIndex(i => (Uri)i.Source.CustomProperties["uri"] == item.MediaUri));
            
            if (MediaElementState.Paused == mediaElement.CurrentState ||
                MediaElementState.Stopped == mediaElement.CurrentState)
            {
                mediaElement.Play();
            }
        }

        /// <summary>
        /// Sends message to the background task to skip to the previous track.
        /// </summary>
        private void prevButton_Click(object sender, RoutedEventArgs e)
        {
            playbackList.MovePrevious();
        }

        /// <summary>
        /// If the task is already running, it will just play/pause MediaPlayer Instance
        /// Otherwise, initializes MediaPlayer Handlers and starts playback
        /// track or to pause if we're already playing.
        /// </summary>
        private void playButton_Click(object sender, RoutedEventArgs e)
        {
            if (MediaElementState.Playing == mediaElement.CurrentState)
            {
                mediaElement.Pause();
            }
            else if (MediaElementState.Paused == mediaElement.CurrentState ||
                MediaElementState.Stopped == mediaElement.CurrentState)
            {
                mediaElement.Play();
            }
        }

        /// <summary>
        /// Tells the background audio agent to skip to the next track.
        /// </summary>
        /// <param name="sender">The button</param>
        /// <param name="e">Click event args</param>
        private void nextButton_Click(object sender, RoutedEventArgs e)
        {
            playbackList.MoveNext();
        }

        private void speedButton_Click(object sender, RoutedEventArgs e)
        {
            // Create menu and add commands
            var popupMenu = new PopupMenu();

            popupMenu.Commands.Add(new UICommand("4.0x", command => mediaElement.PlaybackRate = 4.0));
            popupMenu.Commands.Add(new UICommand("2.0x", command => mediaElement.PlaybackRate = 2.0));
            popupMenu.Commands.Add(new UICommand("1.5x", command => mediaElement.PlaybackRate = 1.5));
            popupMenu.Commands.Add(new UICommand("1.0x", command => mediaElement.PlaybackRate = 1.0));
            popupMenu.Commands.Add(new UICommand("0.5x", command => mediaElement.PlaybackRate = 0.5));

            // Get button transform and then offset it by half the button
            // width to center. This will show the popup just above the button.
            var button = (Button)sender;
            var transform = button.TransformToVisual(null);
            var point = transform.TransformPoint(new Point(button.Width / 2, 0));
            
            // Show popup
            var ignoreAsyncResult = popupMenu.ShowAsync(point);

        }

        private void UpdateTransportControls(MediaElementState state)
        {
            nextButton.IsEnabled = true;
            prevButton.IsEnabled = true;
            if (state == MediaElementState.Playing)
            {
                playButton.Content = "| |";     // Change to pause button
            }
            else
            {
                playButton.Content = ">";     // Change to play button
            }
        }
    }
}