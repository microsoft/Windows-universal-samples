using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();
        }

        private Random _rand = new Random();

        private IList<string> _lines = null;

        public ObservableCollection<ListItemData> Items { get; set; } = new ObservableCollection<ListItemData>();

        public DependencyProperty UseAutoRefreshProperty = DependencyProperty.Register("UseAutoRefresh", typeof(bool), typeof(MainPage), new PropertyMetadata(false));
        public bool UseAutoRefresh
        {
            get { return (bool)GetValue(UseAutoRefreshProperty); }
            set { SetValue(UseAutoRefreshProperty, value); }
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            StorageFile inputFile = await StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///Assets/lorem/lorem.txt"));
            _lines = await FileIO.ReadLinesAsync(inputFile);

            // Fetch and insert the initial batch of items.
            await FetchAndInsertItemsAsync(_rand.Next(1, 21));
        }

        private async Task FetchAndInsertItemsAsync(int updateCount)
        {
            if (_lines == null)
            {
                // Not initialized yet.
                return;
            }

            // Show the status bar progress indicator, if available.
            Windows.UI.ViewManagement.StatusBar statusBar = null;
            if (Windows.Foundation.Metadata.ApiInformation.IsTypePresent("Windows.UI.ViewManagement.StatusBar"))
            {
                statusBar = Windows.UI.ViewManagement.StatusBar.GetForCurrentView();
            }

            if (statusBar != null)
            {
                var task = statusBar.ProgressIndicator.ShowAsync();
            }

            // Simulate delay while we go fetch new items.
            await Task.Delay(500);

            for (int i = 0; i < updateCount; ++i)
            {
                Items.Insert(0, GetNextItem());
            }

            if (statusBar != null)
            {
                var task = statusBar.ProgressIndicator.HideAsync();
            }
        }

        private ListItemData GetNextItem()
        {
            return new ListItemData()
            {
                Image = string.Format("Assets\\lorem\\LandscapeImage{0}.jpg", _rand.Next(1, 26)),
                Header = RandomSentence(),
                Attribution = RandomSentence(),
                Body = RandomSentence()
            };
        }

        private string RandomSentence()
        {
            return _lines[_rand.Next(_lines.Count)];
        }

        private async void listView_RefreshRequested(object sender, RefreshableListView.RefreshRequestedEventArgs e)
        {
            using (Deferral deferral = listView.AutoRefresh ? e.GetDeferral() : null)
            {
                await FetchAndInsertItemsAsync(_rand.Next(1, 5));

                if (SpinnerStoryboard.GetCurrentState() != Windows.UI.Xaml.Media.Animation.ClockState.Stopped)
                {
                    SpinnerStoryboard.Stop();
                }
            }
        }

        private void listView_PullProgressChanged(object sender, RefreshableListView.RefreshProgressEventArgs e)
        {
            if (e.IsRefreshable)
            {
                if (e.PullProgress == 1)
                {
                    // Progress = 1.0 means that the refresh has been triggered.
                    if (SpinnerStoryboard.GetCurrentState() == Windows.UI.Xaml.Media.Animation.ClockState.Stopped)
                    {
                        SpinnerStoryboard.Begin();
                    }
                }
                else if (SpinnerStoryboard.GetCurrentState() != Windows.UI.Xaml.Media.Animation.ClockState.Stopped)
                {
                    SpinnerStoryboard.Stop();
                }
                else
                {
                    // Turn the indicator by an amount proportional to the pull progress.
                    SpinnerTransform.Angle = e.PullProgress * 360;
                }
            }
        }

        private void AutoRefreshToggle_Click(object sender, RoutedEventArgs e)
        {
            listView.AutoRefresh = ((CheckBox)sender).IsChecked.Value;
        }
    }
}
