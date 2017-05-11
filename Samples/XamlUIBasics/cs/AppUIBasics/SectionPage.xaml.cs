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
using AppUIBasics.Data;
using System;
using System.Linq;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Animation;
using Windows.UI.Xaml.Navigation;


// The Section Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234229

namespace AppUIBasics
{
    /// <summary>
    /// A page that displays an overview of a single group, including a preview of the items
    /// within the group.
    /// </summary>
    public sealed partial class SectionPage : Page
    {
        private NavigationHelper navigationHelper;
        private ObservableDictionary defaultViewModel = new ObservableDictionary();
        private string itemId;
        ControlInfoDataGroup group;

        /// <summary>
        /// NavigationHelper is used on each page to aid in navigation and
        /// process lifetime management
        /// </summary>
        public NavigationHelper NavigationHelper
        {
            get { return this.navigationHelper; }
        }

        /// <summary>
        /// This can be changed to a strongly typed view model.
        /// </summary>
        public ObservableDictionary DefaultViewModel
        {
            get { return this.defaultViewModel; }
        }

        public ControlInfoDataGroup Group
        {
            get { return group; }
            set { group = value; }
        }


        public SectionPage()
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
            var group = await ControlInfoDataSource.GetGroupAsync((String)e.NavigationParameter);
            Group = group;
            Bindings.Update();
            ((NavigationMenuItem)NavigationRootPage.Current.NavigationView.MenuItems.FirstOrDefault(m => m.Tag?.ToString() == group.UniqueId)).IsSelected = true;
            NavigationRootPage.Current.NavigationView.Header = group?.Title;
        }

        /// <summary>
        /// Invoked when an item is clicked.
        /// </summary>
        /// <param name="sender">The GridView displaying the item clicked.</param>
        /// <param name="e">Event data that describes the item clicked.</param>
        void ItemView_ItemClick(object sender, ItemClickEventArgs e)
        {
            // Navigate to the appropriate destination page, configuring the new page
            // by passing required information as a navigation parameter
            itemId = ((ControlInfoDataItem)e.ClickedItem).UniqueId;
            var container = itemGridView.ContainerFromItem(e.ClickedItem) as GridViewItem;
            if (container != null)
            {
                itemGridView.PrepareConnectedAnimation("controlAnimation", (ControlInfoDataItem)e.ClickedItem, "controlRoot");
            }
            this.Frame.Navigate(typeof(ItemPage), itemId);
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
        async private void itemGridView_Loaded(object sender, RoutedEventArgs e)
        {
            if (itemId != null)
            {
                var item = this.Group.Items.FirstOrDefault(s => s.UniqueId == itemId);
                if (item != null)
                {
                    itemGridView.ScrollIntoView(item);
                    if (NavigationRootPage.Current.IsFocusSupported)
                    {
                        ((GridViewItem)itemGridView.ContainerFromItem(item))?.Focus(FocusState.Programmatic);
                    }
                    ConnectedAnimation animation =
                        ConnectedAnimationService.GetForCurrentView().GetAnimation("controlAnimation");
                    if (animation != null)
                    {
                        await itemGridView.TryStartConnectedAnimationAsync(
                            animation, item, "controlRoot");
                    }
                }
            }
        }
        #endregion

        private void itemGridView_KeyDown(object sender, KeyRoutedEventArgs e)
        {
            if (e.Key == Windows.System.VirtualKey.Up)
            {
                var nextElement = FocusManager.FindNextElement(FocusNavigationDirection.Up);
                if (nextElement.GetType() != typeof(TextBox))
                {
                    NavigationRootPage.Current.PageHeader.Focus(FocusState.Programmatic);
                }
            }
        }
    }
}