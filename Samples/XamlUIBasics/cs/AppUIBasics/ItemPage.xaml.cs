//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using AppUIBasics.Common;
using AppUIBasics.ControlPages;
using AppUIBasics.Data;
using System;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Navigation;

// The Item Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234232

namespace AppUIBasics
{
    /// <summary>
    /// A page that displays details for a single item within a group.
    /// </summary>
    public partial class ItemPage : Page
    {
        private NavigationHelper navigationHelper;
        private ControlInfoDataItem item;

        /// <summary>
        /// NavigationHelper is used on each page to aid in navigation and 
        /// process lifetime management
        /// </summary>
        public NavigationHelper NavigationHelper
        {
            get { return this.navigationHelper; }
        }

        public ControlInfoDataItem Item
        {
            get { return item; }
            set { item = value; }
        }

        public CommandBar BottomCommandBar
        {
            get { return bottomCommandBar; }
        }

        public ItemPage()
        {
            this.InitializeComponent();
            this.navigationHelper = new NavigationHelper(this);
            this.navigationHelper.LoadState += navigationHelper_LoadState;
        }

        /// <summary>
        /// Populates the page with content passed during navigation.  Any saved state is also
        /// provided when recreating a page from a prior session.
        /// </summary>
        /// <param name="sender">
        /// The source of the event; typically <see cref="NavigationHelper"/>
        /// </param>
        /// <param name="e">Event data that provides both the navigation parameter passed to
        /// <see cref="Frame.Navigate(Type, Object)"/> when this page was initially requested and
        /// a dictionary of state preserved by this page during an earlier
        /// session.  The state will be null the first time a page is visited.</param>
        private async void navigationHelper_LoadState(object sender, LoadStateEventArgs e)
        {
            var item = await ControlInfoDataSource.GetItemAsync((String)e.NavigationParameter);

            if (item != null)
            {
                Item = item;

                // Load control page into frame.
                var loader = Windows.ApplicationModel.Resources.ResourceLoader.GetForCurrentView();

                string pageRoot = loader.GetString("PageStringRoot");

                string pageString = pageRoot + item.UniqueId + "Page";
                Type pageType = Type.GetType(pageString);

                if (pageType != null)
                {
                    this.contentFrame.Navigate(pageType);
                }

                if(item.Title == "AppBar")
                {
                    //Child pages don't follow the visible bounds, so we need to add margin to account for this
                    header.Margin = new Thickness(0, 24, 0, 0);
                }
            }
        }

        private void ThemeToggleButton_Checked(object sender, RoutedEventArgs e)
        {
            this.RequestedTheme = ElementTheme.Light;
        }

        private void ThemeToggleButton_Unchecked(object sender, RoutedEventArgs e)
        {
            this.RequestedTheme = ElementTheme.Dark;
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            base.OnNavigatingFrom(e);
            this.RequestedTheme = ElementTheme.Dark;
        }

        private void SearchButton_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(SearchResultsPage));
        }

        private void HelpButton_Click(object sender, RoutedEventArgs e)
        {
            ShowHelp();
            bottomCommandBar.IsOpen = false;
        }

        private void AppBarButton_GotFocus(object sender, RoutedEventArgs e)
        {
            // The default gamepad navigation is awkward in this app due to the way the app lays
            // out its pages. It is common for users to overshoot when navigating with the gamepad.
            // If users press Down too many times, they end up on the command bar buttons in
            // the bottom right corner of the page. Pressing Up doesn't return focus anywhere
            // close to where it came from because pressing Up puts focus on the search box
            // because that's the next control in the upward direction.
            //
            // Ideally, we would revise the page layout so that there is a clear next control
            // in each direction. Here, we programmatically set the XYFocusUp property
            // to the control that is closest to the command bar, which is usually the
            // bottom-most control in the content frame.

            var transform = bottomCommandBar.TransformToVisual(null);
            // Calculate the rectangle that describes the top edge of the bottom command bar.
            var rect = new Rect(0, 0, bottomCommandBar.ActualWidth, 0);
            rect = transform.TransformBounds(rect);
            var destinationElement = FocusManager.FindNextFocusableElement(FocusNavigationDirection.Up, rect);
            searchButton.XYFocusUp = destinationElement;
        }

        protected void RelatedControl_Click(object sender, RoutedEventArgs e)
        {
            ButtonBase b = (ButtonBase)sender;

            NavigationRootPage.RootFrame.Navigate(typeof(ItemPage), b.Content.ToString());
        }

        private void ShowHelp()
        {
            var loader = Windows.ApplicationModel.Resources.ResourceLoader.GetForCurrentView();

            string HTMLOpenTags = loader.GetString("HTMLOpenTags");
            string HTMLCloseTags = loader.GetString("HTMLCloseTags");
            
            contentWebView.NavigateToString(HTMLOpenTags + Item.Content + HTMLCloseTags);

            if (!helpPopup.IsOpen)
            {
                rootPopupBorder.Width = 346;
                rootPopupBorder.Height = this.ActualHeight;
                helpPopup.HorizontalOffset = Window.Current.Bounds.Width - 346;
                helpPopup.IsOpen = true;
            }
        }

        private void SearchBox_QuerySubmitted(SearchBox sender, SearchBoxQuerySubmittedEventArgs args)
        {

            this.Frame.Navigate(typeof(SearchResultsPage), args.QueryText);

        }

        #region NavigationHelper registration

        /// The methods provided in this section are simply used to allow
        /// NavigationHelper to respond to the page's navigation methods.
        /// 
        /// Page specific logic should be placed in event handlers for the  
        /// <see cref="GridCS.Common.NavigationHelper.LoadState"/>
        /// and <see cref="GridCS.Common.NavigationHelper.SaveState"/>.
        /// The navigation parameter is available in the LoadState method 
        /// in addition to page state preserved during an earlier session.


        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            navigationHelper.OnNavigatedTo(e);
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            navigationHelper.OnNavigatedFrom(e);
        }

        #endregion
    }
}