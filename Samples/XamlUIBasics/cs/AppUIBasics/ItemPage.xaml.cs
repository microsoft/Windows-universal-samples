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
using System.Collections.Generic;
using System.Linq;
using Windows.ApplicationModel.Core;
using Windows.Foundation;
using Windows.System;
using Windows.UI.Composition;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Hosting;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media.Animation;
using Windows.UI.Xaml.Media.Imaging;
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
        Compositor _compositor;
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


        public ItemPage()
        {
            this.InitializeComponent();
            this.navigationHelper = new NavigationHelper(this);
            this.navigationHelper.LoadState += navigationHelper_LoadState;
            _compositor = ElementCompositionPreview.GetElementVisual(this).Compositor;
            Loaded += ItemPage_Loaded;
            CoreApplicationViewTitleBar coreTitleBar = CoreApplication.GetCurrentView().TitleBar;
            coreTitleBar.IsVisibleChanged += CoreTitleBar_IsVisibleChanged;
        }

        private void ItemPage_Loaded(object sender, RoutedEventArgs e)
        {
            SetInitialVisuals();
        }

        private void CoreTitleBar_IsVisibleChanged(CoreApplicationViewTitleBar sender, object args)
        {
            svPanel.Margin = NavigationRootPage.Current.DeviceFamily == DeviceType.Xbox ? new Thickness(0, 0, 48, 0) : new Thickness(0);
        }

        public void SetInitialVisuals()
        {
            NavigationRootPage.Current.PageHeader.TopCommandBar.Visibility = Visibility.Visible;
            IEnumerable<RadioButton> buttons = NavigationRootPage.Current.PageHeader.ThemeFlyout.Content.GetDescendantsOfType<RadioButton>();
            foreach (var button in buttons)
            {
                var selectedTheme = (this.RequestedTheme == ElementTheme.Default) ? App.Current.RequestedTheme.ToString() : this.RequestedTheme.ToString();
                button.IsChecked = (button.Tag.ToString() == selectedTheme);
                button.Checked += RadioButton_Checked;
            }
            NavigationRootPage.Current.PageHeader.IconUri = new BitmapImage(new Uri(item?.ImagePath, UriKind.RelativeOrAbsolute));
            if (NavigationRootPage.Current.IsFocusSupported)
            {
                this.Focus(FocusState.Programmatic);
            }
            svPanel.Margin = NavigationRootPage.Current.DeviceFamily == DeviceType.Xbox ? new Thickness(0, 0, 48, 0) : new Thickness(0);
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

                NavigationRootPage.Current.NavigationView.Header = item?.Title;
                if (item.IsNew && NavigationRootPage.Current.CheckNewControlSelected())
                {
                    return;
                }

                ControlInfoDataGroup group = await ControlInfoDataSource.GetGroupFromItemAsync((String)e.NavigationParameter);
                var menuItem = NavigationRootPage.Current.NavigationView.MenuItems.Cast<NavigationViewItem>().FirstOrDefault(m => m.Tag?.ToString() == group.UniqueId);
                if (menuItem != null)
                {
                    menuItem.IsSelected = true;
                }
            }
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

        protected void RelatedControl_Click(object sender, RoutedEventArgs e)
        {
            ButtonBase b = (ButtonBase)sender;

            NavigationRootPage.RootFrame.Navigate(typeof(ItemPage), b.DataContext.ToString());
        }

        private void ShowHelp()
        {
            var loader = Windows.ApplicationModel.Resources.ResourceLoader.GetForCurrentView();

            string HTMLOpenTags = loader.GetString("HTMLOpenTags");
            string HTMLCloseTags = loader.GetString("HTMLCloseTags");

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

            //Connected Animation

            PopOutStoryboard.Begin();
            PopOutStoryboard.Completed += (sender1_, e1_) =>
            {
                PopInStoryboard.Begin();
            };
            if (NavigationRootPage.Current.PageHeader != null)
            {
                var connectedAnimationService = ConnectedAnimationService.GetForCurrentView();
                ConnectedAnimation connectedAnimation = connectedAnimationService.GetAnimation("controlAnimation");
                RelativePanel relativePanel = NavigationRootPage.Current.PageHeader.Content.GetDescendantsOfType<RelativePanel>().FirstOrDefault();
                if (connectedAnimation != null)
                {
                    connectedAnimation.TryStart(relativePanel, new UIElement[] { subTitleText });
                }
            }
            navigationHelper.OnNavigatedTo(e);
        }


        private void RadioButton_Checked(object sender, RoutedEventArgs e)
        {
            this.RequestedTheme = (ElementTheme)((RadioButton)(sender)).Tag;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            NavigationRootPage.Current.PageHeader.TopCommandBar.Visibility = Visibility.Collapsed;
            IEnumerable<RadioButton> buttons = NavigationRootPage.Current.PageHeader.TopCommandBar.GetDescendantsOfType<RadioButton>();
            foreach (var button in buttons)
            {
                button.Checked -= RadioButton_Checked;
            }
            NavigationRootPage.Current.PageHeader.IconUri = null;
            //Reverse Connected Animation
            if (e.SourcePageType == typeof(MainPage) || e.SourcePageType == typeof(NewControlsPage))
            {
                RelativePanel relativePanel = NavigationRootPage.Current.PageHeader.Content.GetDescendantsOfType<RelativePanel>().FirstOrDefault();
                ConnectedAnimationService.GetForCurrentView().PrepareToAnimate("controlAnimation", relativePanel);
            }
            navigationHelper.OnNavigatedFrom(e);
        }

        #endregion

        private void svPanel_Loaded(object sender, RoutedEventArgs e)
        {
            svPanel.XYFocusDown = contentFrame.GetDescendantsOfType<Control>().FirstOrDefault(c => c.IsTabStop) ?? svPanel.GetDescendantsOfType<Control>().FirstOrDefault(c => c.IsTabStop);
        }

        private void List_Loaded(object sender, RoutedEventArgs e)
        {
            ListView listview = sender as ListView;
            ListViewItem item = (ListViewItem)listview.ContainerFromItem(listview.Items.LastOrDefault());
            if (item != null)
                item.XYFocusDown = item;
        }

        private async void DocsList_ItemClick(object sender, ItemClickEventArgs e)
        {
            await Launcher.LaunchUriAsync(new Uri(((ControlInfoDocLink)e.ClickedItem).Uri));
        }

        private void ListView_ItemClick(object sender, ItemClickEventArgs e)
        {
            NavigationRootPage.RootFrame.Navigate(typeof(ItemPage), e.ClickedItem?.ToString());
        }

        private void svPanel_GotFocus(object sender, RoutedEventArgs e)
        {
            if (e.OriginalSource == sender && NavigationRootPage.Current.IsFocusSupported)
            {
                bool isElementFound = false;
                var elements = contentFrame.GetDescendantsOfType<Control>().Where(c => c.IsTabStop);
                foreach (var element in elements)
                {
                    Rect elementRect = element.TransformToVisual(svPanel).TransformBounds(new Rect(0.0, 0.0, element.ActualWidth, element.ActualHeight));
                    Rect panelRect = new Rect(0.0, 70.0, svPanel.ActualWidth, svPanel.ActualHeight);

                    if (elementRect.Top < panelRect.Bottom)
                    {
                        element.Focus(FocusState.Programmatic);
                        isElementFound = true;
                        break;
                    }
                }
                if (!isElementFound)
                {
                    svPanel.UseSystemFocusVisuals = true;
                }
            }
        }

        private void svPanel_KeyDown(object sender, KeyRoutedEventArgs e)
        {
            if (e.Key == VirtualKey.Up)
            {
                var nextElement = FocusManager.FindNextElement(FocusNavigationDirection.Up);
                if (nextElement.GetType() == typeof(NavigationViewItem))
                {
                    NavigationRootPage.Current.PageHeader.Focus(FocusState.Programmatic);
                }
                else
                {
                    FocusManager.TryMoveFocus(FocusNavigationDirection.Up);
                }
            }

        }
    }
}