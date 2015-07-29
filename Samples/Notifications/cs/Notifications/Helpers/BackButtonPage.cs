using Windows.UI.Core;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace Notifications.Helpers
{
    public class BackButtonPage : Page
    {
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            BackButtonVisibility = base.Frame.CanGoBack ? AppViewBackButtonVisibility.Visible : AppViewBackButtonVisibility.Collapsed;

            SystemNavigationManager.GetForCurrentView().BackRequested += BackButtonPage_BackRequested;

            base.OnNavigatedTo(e);
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            base.OnNavigatedFrom(e);

            SystemNavigationManager.GetForCurrentView().BackRequested -= BackButtonPage_BackRequested;
        }

        private void BackButtonPage_BackRequested(object sender, BackRequestedEventArgs e)
        {
            OnBackRequested(sender, e);
        }

        protected virtual void OnBackRequested(object sender, BackRequestedEventArgs e)
        {
            if (base.Frame.CanGoBack)
            {
                e.Handled = true;
                base.Frame.GoBack();
            }
        }

        public AppViewBackButtonVisibility BackButtonVisibility
        {
            get { return SystemNavigationManager.GetForCurrentView().AppViewBackButtonVisibility; }
            set { SystemNavigationManager.GetForCurrentView().AppViewBackButtonVisibility = value; }
        }
    }
}
