using SDKTemplate.Common;
using System;
using System.Collections.Generic;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Core;

namespace SDKTemplate
{
    public sealed partial class SubPage : Page
    {
        private readonly NavigationHelper navigationHelper;
        private int tapCount;

        public SubPage()
        {
            this.InitializeComponent();
            this.navigationHelper = new NavigationHelper(this);
            this.navigationHelper.LoadState += LoadState;
            this.navigationHelper.SaveState += SaveState;

            SystemNavigationManager.GetForCurrentView().BackRequested += Navigation_BackRequested;
        }

        #region NavigationHelper registration

        /// <summary>
        /// The methods provided in this section are simply used to allow
        /// NavigationHelper to respond to the page's navigation methods.
        /// <para>
        /// Page specific logic should be placed in event handlers for the  
        /// <see cref="NavigationHelper.LoadState"/>
        /// and <see cref="NavigationHelper.SaveState"/>.
        /// The navigation parameter is available in the LoadState method 
        /// in addition to page state preserved during an earlier session.
        /// </para>
        /// </summary>
        /// <param name="e">Provides data for navigation methods and event
        /// handlers that cannot cancel the navigation request.</param>

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            this.TitleTextBlock.Text = e.Parameter.ToString();

            this.navigationHelper.OnNavigatedTo(e);

            SystemNavigationManager.GetForCurrentView().AppViewBackButtonVisibility =
                this.Frame.CanGoBack ? AppViewBackButtonVisibility.Visible : AppViewBackButtonVisibility.Collapsed;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            this.navigationHelper.OnNavigatedFrom(e);
        }

        private void LoadState(object sender, LoadStateEventArgs e)
        {
            tapCount = 0;
            if (e.PageState != null && e.PageState.ContainsKey("tapCount"))
            {
                tapCount = (int)e.PageState["tapCount"];
            }
            TapCountRun.Text = tapCount.ToString();
        }

        private void SaveState(object sender, SaveStateEventArgs e)
        {
            e.PageState["tapCount"] = tapCount;
        }
        #endregion

        /// <summary>
        /// This method facilitates navigation when the back button is pressed.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Navigation_BackRequested(object sender, BackRequestedEventArgs e)
        {
            if (!e.Handled & this.Frame.CanGoBack)
            {
                e.Handled = true;
                this.Frame.GoBack();
            }
        }

        private void Page_Tapped(object sender, Windows.UI.Xaml.Input.TappedRoutedEventArgs e)
        {
            tapCount++;
            TapCountRun.Text = tapCount.ToString();
        }
    }
}
