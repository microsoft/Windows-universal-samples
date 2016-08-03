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
using System.Diagnostics;
using Windows.Foundation;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.UI.Core;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// Demonstrates playing video lists using MediaPlaybackList.
    /// </summary>
    public sealed partial class Scenario6 : Page
    {
        private MainPage rootPage = MainPage.Current;
        private MediaPlaybackList playbackList = new MediaPlaybackList();
        private MediaPlayer mp;

        public Scenario6()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Create a static playback list
            InitializePlaybackList();

            //Create the MediaPlayer:
            mp = new MediaPlayer();

            // Subscribe to MediaPlayer PlaybackState changed events
            mp.PlaybackSession.PlaybackStateChanged += PlaybackSession_PlaybackStateChanged;

            // Subscribe to list UI changes
            playlistView.ItemClick += PlaylistView_ItemClick;

            //Attach the player to the MediaPlayerElement:
            mediaPlayerElement.SetMediaPlayer(mp);

            // Set list for playback
            mp.Source = playbackList;
        }

        void InitializePlaybackList()
        {
            // Initialize the playlist data/view model.
            // In a production app your data would be sourced from a data store or service.

            // Add content to the ListView and to the MediaPlaybackList.
            MediaModel media = new MediaModel(rootPage.MultiTrackVideoMediaUri)
            {
                Title = "Fitness",
                ArtUri = new Uri("ms-appx:///Assets/Media/multivideo.jpg")
            };
            playlistView.Items.Add(media);
            playbackList.Items.Add(media.MediaPlaybackItem);

            media = new MediaModel(rootPage.CaptionedMediaUri)
            {
                Title = "Elephant's Dream",
                ArtUri = new Uri("ms-appx:///Assets/Media/ElephantsDream.jpg")
            };
            playlistView.Items.Add(media);
            playbackList.Items.Add(media.MediaPlaybackItem);

            media = new MediaModel(rootPage.SintelMediaUri)
            {
                Title = "Sintel",
                ArtUri = new Uri("ms-appx:///Assets/Media/sintel.jpg")
            };
            playlistView.Items.Add(media);
            playbackList.Items.Add(media.MediaPlaybackItem);

            // Subscribe for changes
            playbackList.CurrentItemChanged += PlaybackList_CurrentItemChanged;

            // Loop
            playbackList.AutoRepeatEnabled = true;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            MediaPlayerHelper.CleanUpMediaPlayerSource(mp);
            playbackList.Items.Clear();
            playlistView.Items.Clear();
        }

        private void PlaybackList_CurrentItemChanged(MediaPlaybackList sender, CurrentMediaPlaybackItemChangedEventArgs args)
        {
            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                // Synchronize our UI with the currently-playing item.
                playlistView.SelectedIndex = (int)sender.CurrentItemIndex;
            });
        }

        /// <summary>
        /// MediaPlayer state changed event handlers.
        /// Note that we can subscribe to events even if Media Player is playing media in background
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        ///
        private async void PlaybackSession_PlaybackStateChanged(MediaPlaybackSession sender, object args)
        {
            await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                var currentState = sender.PlaybackState;

                // Update state label
                txtCurrentState.Text = currentState.ToString();

                // Update controls
                UpdateTransportControls(currentState);
            });
        }

        private void PlaylistView_ItemClick(object sender, ItemClickEventArgs e)
        {
            var item = e.ClickedItem as MediaModel;
            Debug.WriteLine("Clicked item: " + item.Title);

            // Start the background task if it wasn't running
            playbackList.MoveTo((uint)playbackList.Items.IndexOf(item.MediaPlaybackItem));

            if (MediaPlaybackState.Paused == mp.PlaybackSession.PlaybackState)
            {
                mp.Play();
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
            if (MediaPlaybackState.Playing == mp.PlaybackSession.PlaybackState)
            {
                mp.Pause();
            }
            else if (MediaPlaybackState.Paused == mp.PlaybackSession.PlaybackState)
            {
                mp.Play();
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

        private async void speedButton_Click(object sender, RoutedEventArgs e)
        {
            // Create menu and add commands
            var popupMenu = new PopupMenu();

            popupMenu.Commands.Add(new UICommand("4.0x", command => mp.PlaybackSession.PlaybackRate = 4.0));
            popupMenu.Commands.Add(new UICommand("2.0x", command => mp.PlaybackSession.PlaybackRate = 2.0));
            popupMenu.Commands.Add(new UICommand("1.5x", command => mp.PlaybackSession.PlaybackRate = 1.5));
            popupMenu.Commands.Add(new UICommand("1.0x", command => mp.PlaybackSession.PlaybackRate = 1.0));
            popupMenu.Commands.Add(new UICommand("0.5x", command => mp.PlaybackSession.PlaybackRate = 0.5));

            // Get button transform and then offset it by half the button
            // width to center. This will show the popup just above the button.
            var button = (Button)sender;
            var transform = button.TransformToVisual(null);
            var point = transform.TransformPoint(new Point(button.ActualWidth / 2, 0));

            // Show popup
            IUICommand result = await popupMenu.ShowAsync(point);
            if (result != null)
            {
                button.Content = result.Label;
            }
        }

        private void UpdateTransportControls(MediaPlaybackState state)
        {
            nextButton.IsEnabled = true;
            prevButton.IsEnabled = true;
            if (state == MediaPlaybackState.Playing)
            {
                playButtonSymbol.Symbol = Symbol.Pause;
            }
            else
            {
                playButtonSymbol.Symbol = Symbol.Play;
            }
        }
    }

    public class MediaModel
    {
        public MediaModel(Uri mediaUri)
        {
            MediaPlaybackItem = new MediaPlaybackItem(MediaSource.CreateFromUri(mediaUri));
        }

        public string Title { get; set; }
        public Uri ArtUri { get; set; }
        public MediaPlaybackItem MediaPlaybackItem { get; private set; }
    }
}