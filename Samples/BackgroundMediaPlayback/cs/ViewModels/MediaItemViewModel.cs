using BackgroundMediaPlayback.DataModels;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Media.Playback;
using Windows.UI.Xaml.Media.Imaging;

namespace BackgroundMediaPlayback.ViewModels
{
    /// <summary>
    /// View model for media items.
    /// </summary>
    /// <remarks>
    /// The view model is backed by a data model (MediaItem)
    /// and a playback model (MediaPlaybackItem).
    /// 
    /// This class currently supports OneWay (read only) binding 
    /// but could be extended to support TwoWay binding to
    /// persist changes back to the data and playback models.
    /// 
    /// The projected view is the same regardless of the type
    /// of media. More sophisticated applications may want to
    /// create separate models and XAML data templates for
    /// unique content display.
    /// </remarks> 
    public class MediaItemViewModel : INotifyPropertyChanged
    {
        MediaListViewModel listViewModel;
        MediaPlaybackItem playbackItem;

        BitmapImage previewImage;

        public event PropertyChangedEventHandler PropertyChanged;

        public MediaItem MediaItem { get; private set; }

        public string Title => MediaItem.Title;

        public BitmapImage PreviewImage
        {
            get { return previewImage; }

            private set
            {
                if (previewImage != value)
                {
                    previewImage = value;
                    RaisePropertyChanged("PreviewImage");
                }
            }
        }

        public MediaPlaybackItem PlaybackItem
        {
            get
            {
                // Already have one then return it
                if (playbackItem != null)
                    return playbackItem;

                // Don't have one, try to rebind to one in the list
                playbackItem = listViewModel.PlaybackList.Items.SingleOrDefault(pi => 
                    (string)pi.Source.CustomProperties[MediaItem.MediaItemIdKey] == MediaItem.ItemId);

                if (playbackItem != null)
                    return playbackItem;

                // Not in the list, make a new one
                playbackItem = MediaItem.ToPlaybackItem();
                RaisePropertyChanged("PlaybackItem");
                return playbackItem;
            }
        }

        public MediaItemViewModel(MediaListViewModel listViewModel, MediaItem mediaItem)
        {
            this.listViewModel = listViewModel;
            MediaItem = mediaItem;

            RaisePropertyChanged("Title");

            // This app caches all images by loading the BitmapImage
            // when the item is created, but production apps would
            // use a more resource friendly paging mechanism or
            // just use PreviewImageUri directly.
            //
            // The reason we cache here is to avoid audio gaps 
            // between tracks on transitions when changing artwork.
            PreviewImage = new BitmapImage();
            PreviewImage.UriSource = mediaItem.PreviewImageUri;
        }

        private void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
