using Windows.UI.Core;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace _360VideoPlayback
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();
            SystemNavigationManager.GetForCurrentView().BackRequested += MainPage_BackRequested;
            this.contentFrame.Navigate(typeof(VideoGallery));
        }

        private void MainPage_BackRequested(object sender, BackRequestedEventArgs e)
        {
            goBack(null, e);
        }

        private void goBack(object sender, BackRequestedEventArgs e)
        {
            if (this.contentFrame.CanGoBack)
            {
                this.contentFrame.GoBack();
                e.Handled = true;
            }
        }
    }
}
