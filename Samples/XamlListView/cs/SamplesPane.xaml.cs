using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ListViewSample
{
    public sealed partial class SamplesPane : UserControl
    {
        public SamplesPane()
        {
            this.InitializeComponent();
        }
        private void NavigateToOptimized(object sender, RoutedEventArgs e)
        {
            ((Frame)Window.Current.Content).Navigate(typeof(SimpleListViewSample));
        }
        private void NavigateToOptimizedGrid(object sender, RoutedEventArgs e)
        {
            ((Frame)Window.Current.Content).Navigate(typeof(SimpleGridViewSample));
        }
        private void NavigateToMasterDetailSelection(object sender, RoutedEventArgs e)
        {
            ((Frame)Window.Current.Content).Navigate(typeof(MasterDetailSelection));
        }
        private void NavigateToEdgeTappedListView(object sender, RoutedEventArgs e)
        {
            ((Frame)Window.Current.Content).Navigate(typeof(TapOnTheEdgeSample));
        }
        private void NavigateToRestoreScrollPosition(object sender, RoutedEventArgs e)
        {
            ((Frame)Window.Current.Content).Navigate(typeof(RestoreScrollPositionSample));
        }
        private void NavigateToHome(object sender, RoutedEventArgs e)
        {
            ((Frame)Window.Current.Content).Navigate(typeof(MainPage));
        }

        private void NavigateToSpecificPosition(object sender, RoutedEventArgs e)
        {
            ((Frame)Window.Current.Content).Navigate(typeof(ScrollIntoViewSample));
        }
    }
}
