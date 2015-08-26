using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Animation;
using Windows.UI.Xaml.Navigation;

namespace ListViewSample
{
    /// <summary>
    /// Page2 is simply a blank page with a button to navigate back to the MainPage. 
    /// It's purpose is just to reset the list on the main page so that the implementation of ListViewPersistenceHelper can be demonstrated
    /// </summary>
    public sealed partial class Page2 : Page
    {
        public Page2()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);

            // Register for hardware and software back request from the system
            SystemNavigationManager.GetForCurrentView().BackRequested += OnHardwareBackRequested;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            base.OnNavigatedFrom(e);
            SystemNavigationManager.GetForCurrentView().BackRequested -= OnHardwareBackRequested;
        }

        private void OnHardwareBackRequested(object sender, BackRequestedEventArgs e)
        {
            // Mark event as handled so we don't get bounced out of the app.
            e.Handled = true;
            OnBackRequested();
        }

        private void OnBackRequested()
        {
            // Page above us will be our master view.
            // Make sure we are using the "drill out" animation in this transition.
            Frame.GoBack(new DrillInNavigationTransitionInfo());
        }

        /// <summary>
        /// This method is a click event handler for the button, and it navigates back to the Main Page
        /// </summary>
        private void BackToMainPage(object sender, RoutedEventArgs e)
        {
            OnBackRequested();
        }
    }
}
