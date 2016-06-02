using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Foundation.Collections;
using Windows.Media.Playback;
using Windows.UI.Core;

namespace BackgroundMediaPlayback.ViewModels
{
    /// <summary>
    /// The view model for the player.
    /// </summary>
    /// <remarks>
    /// The view disables the ability to skip during a transition or when
    /// the playback list is empty.
    /// </remarks>
    public class PlayerViewModel : INotifyPropertyChanged, IDisposable
    {
        bool disposed;
        MediaPlayer player;
        CoreDispatcher dispatcher;
        MediaPlaybackList subscribedPlaybackList;

        MediaListViewModel mediaList;
        bool canSkipNext;
        bool canSkipPrevious;

        public PlaybackSessionViewModel PlaybackSession { get; private set; }

        public bool CanSkipNext
        {
            get { return canSkipNext; }
            set
            {
                if (canSkipNext != value)
                {
                    canSkipNext = value;
                    RaisePropertyChanged("CanSkipNext");
                }
            }
        }

        public bool CanSkipPrevious
        {
            get { return canSkipPrevious; }
            set
            {
                if (canSkipPrevious != value)
                {
                    canSkipPrevious = value;
                    RaisePropertyChanged("CanSkipPrevious");
                }
            }
        }

        public IEnumerable<string> AudioCategories
        {
            get { return Enum.GetNames(typeof(MediaPlayerAudioCategory)); }
        }

        public string SelectedAudioCategory
        {
            get { return Enum.GetName(typeof(MediaPlayerAudioCategory), player.AudioCategory); }
            set
            {
                if (Enum.GetName(typeof(MediaPlayerAudioCategory), player.AudioCategory) != value)
                {
                    player.AudioCategory = (MediaPlayerAudioCategory)Enum.Parse(typeof(MediaPlayerAudioCategory), value);
                    RaisePropertyChanged("SelectedAudioCategory");
                }
            }
        }

        public MediaListViewModel MediaList
        {
            get { return mediaList; }
            set
            {
                if (mediaList != value)
                {
                    if (subscribedPlaybackList != null)
                    {
                        subscribedPlaybackList.CurrentItemChanged -= SubscribedPlaybackList_CurrentItemChanged;
                        subscribedPlaybackList.Items.VectorChanged -= Items_VectorChanged;
                        subscribedPlaybackList = null;
                    }

                    mediaList = value;

                    if (mediaList != null)
                    {
                        if (player.Source != mediaList.PlaybackList)
                            player.Source = mediaList.PlaybackList;

                        subscribedPlaybackList = mediaList.PlaybackList;
                        subscribedPlaybackList.CurrentItemChanged += SubscribedPlaybackList_CurrentItemChanged;
                        subscribedPlaybackList.Items.VectorChanged += Items_VectorChanged;
                        HandlePlaybackListChanges(subscribedPlaybackList.Items);
                    }
                    else
                    {
                        CanSkipNext = false;
                        CanSkipPrevious = false;
                    }

                    RaisePropertyChanged("MediaList");
                }
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public PlayerViewModel(MediaPlayer player, CoreDispatcher dispatcher)
        {
            this.player = player;
            this.dispatcher = dispatcher;

            PlaybackSession = new PlaybackSessionViewModel(player.PlaybackSession, dispatcher);
        }

        public void TogglePlayPause()
        {
            switch (player.PlaybackSession.PlaybackState)
            {
                case MediaPlaybackState.Playing:
                    player.Pause();
                    break;
                case MediaPlaybackState.Paused:
                    player.Play();
                    break;
            }
        }

        public void SkipNext()
        {
            if (!CanSkipNext)
                return;

            var playbackList = player.Source as MediaPlaybackList;
            if (playbackList == null)
                return;

            playbackList.MoveNext();
            CanSkipNext = false;
        }

        public void SkipPrevious()
        {
            if (!CanSkipPrevious)
                return;

            var playbackList = player.Source as MediaPlaybackList;
            if (playbackList == null)
                return;

            playbackList.MovePrevious();
            CanSkipPrevious = false;
        }

        private async void Items_VectorChanged(IObservableVector<MediaPlaybackItem> sender, IVectorChangedEventArgs args)
        {
            if (disposed) return;
            await dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                if (disposed) return;
                HandlePlaybackListChanges(sender);
            });
        }

        private async void SubscribedPlaybackList_CurrentItemChanged(MediaPlaybackList sender, CurrentMediaPlaybackItemChangedEventArgs args)
        {
            if (disposed) return;
            await dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                if (disposed) return;
                HandlePlaybackListChanges(sender.Items);
            });
        }

        private void HandlePlaybackListChanges(IObservableVector<MediaPlaybackItem> vector)
        {
            if (vector.Count > 0)
            {
                CanSkipNext = true;
                CanSkipPrevious = true;
            }
            else
            {
                CanSkipNext = false;
                CanSkipPrevious = false;
            }
        }

        private void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
      
        public void Dispose()
        {
            if (disposed)
                return;

            if (MediaList != null)
            {
                MediaList.Dispose();
                MediaList = null; // Setter triggers vector unsubscribe logic
            }

            PlaybackSession.Dispose();

            disposed = true;
        }
    }
}
