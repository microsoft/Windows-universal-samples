using System;
using System.Collections.ObjectModel;
using Windows.ApplicationModel.Core;
using Windows.Graphics.Holographic;
using Windows.Perception.Spatial.Surfaces;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;


// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace _360VideoPlayback
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class VideoGallery : Page
    {
        static public int mainviewId = 0;

        public VideoGallery()
        {
            this.InitializeComponent();
            mainviewId = ApplicationView.GetForCurrentView().Id;
            this.Loaded += VideoGallery_Loaded;
        }

        async private void GalleryGridView_ItemClick(object sender, ItemClickEventArgs e)
        {
            VideoItem selectedVideo = (VideoItem)e.ClickedItem;
            if (selectedVideo != null)
            {                
                if ((HolographicSpace.IsSupported && HolographicSpace.IsAvailable)
                    || SpatialSurfaceObserver.IsSupported())
                {
                    var appViewSource = new AppViewSource(selectedVideo.SourceUri);
                    var appView = CoreApplication.CreateNewView(appViewSource);
                    await appView.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, async () =>
                    {
                        int appviewId = ApplicationView.GetForCurrentView().Id;
                        bool viewShown = await ApplicationViewSwitcher.TryShowAsStandaloneAsync(appviewId, ViewSizePreference.Default, mainviewId, ViewSizePreference.Default);
                        System.Diagnostics.Debug.Assert(viewShown);
                    });
                }
                else
                {
                    Frame.Navigate(typeof(PlaybackPage), selectedVideo.SourceUri);
                }
            }
        }

        private class VideoItemInfo
        {
            public string Title;
            public string Source;
        }

        static VideoItemInfo[] sampleVideos =
        {
            // You can add more sample videos here.
            new VideoItemInfo { Title = "Sample Video", Source = "https://mediaplatstorage1.blob.core.windows.net/windows-universal-samples-media/SampleVideo.mp4" }
        };

        private void VideoGallery_Loaded(object sender, RoutedEventArgs e)
        {
            ObservableCollection<VideoItem> videoList = new ObservableCollection<VideoItem>();
            foreach (var info in sampleVideos)
            {
                videoList.Add(new VideoItem(info.Title, new Uri(info.Source)));
            }

            GalleryGridView.ItemsSource = videoList;
        }
    }
}
