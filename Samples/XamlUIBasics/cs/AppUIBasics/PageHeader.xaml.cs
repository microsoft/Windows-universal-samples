using AppUIBasics.Common;
using AppUIBasics.Data;
using System;
using System.Collections.Generic;
using System.Linq;
using Windows.ApplicationModel.Core;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;


// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace AppUIBasics
{
    public sealed partial class PageHeader : UserControl
    {
        public static readonly DependencyProperty TitleProperty = DependencyProperty.Register("Title", typeof(object), typeof(PageHeader), new PropertyMetadata(null));
        public object Title
        {
            get { return GetValue(TitleProperty); }
            set { SetValue(TitleProperty, value); }
        }

        public ImageSource IconUri
        {
            get { return (ImageSource)GetValue(IconUriProperty); }
            set { SetValue(IconUriProperty, value); }
        }

        // Using a DependencyProperty as the backing store for IconUri.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IconUriProperty =
            DependencyProperty.Register("IconUri", typeof(ImageSource), typeof(PageHeader), new PropertyMetadata(null));


        public double BackgroundColorOpacity
        {
            get { return (double)GetValue(BackgroundColorOpacityProperty); }
            set { SetValue(BackgroundColorOpacityProperty, value); }
        }

        // Using a DependencyProperty as the backing store for BackgroundColorOpacity.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty BackgroundColorOpacityProperty =
            DependencyProperty.Register("BackgroundColorOpacity", typeof(double), typeof(PageHeader), new PropertyMetadata(0.0));

        public double AcrylicOpacity
        {
            get { return (double)GetValue(AcrylicOpacityProperty); }
            set { SetValue(AcrylicOpacityProperty, value); }
        }

        // Using a DependencyProperty as the backing store for BackgroundColorOpacity.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty AcrylicOpacityProperty =
            DependencyProperty.Register("AcrylicOpacity", typeof(double), typeof(PageHeader), new PropertyMetadata(0.3));


        public bool ShowAcrylicBehindHeader
        {
            get { return (bool)GetValue(ShowAcrylicBehindHeaderProperty); }
            set { SetValue(ShowAcrylicBehindHeaderProperty, value); }
        }

        // Using a DependencyProperty as the backing store for BackgroundColorOpacity.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty ShowAcrylicBehindHeaderProperty =
            DependencyProperty.Register("ShowAcrylicBehindHeader", typeof(Visibility), typeof(PageHeader), new PropertyMetadata(Visibility.Visible));


        public static readonly DependencyProperty WideLayoutThresholdProperty = DependencyProperty.Register("WideLayoutThreshold", typeof(double), typeof(PageHeader), new PropertyMetadata(600));
        public double WideLayoutThreshold
        {
            get { return (double)GetValue(WideLayoutThresholdProperty); }
            set
            {
                SetValue(WideLayoutThresholdProperty, value);
                WideLayoutTrigger.MinWindowWidth = value;
            }
        }


        public SolidColorBrush BackgroundBrush
        {
            get { return (SolidColorBrush)GetValue(BackgroundBrushProperty); }
            set { SetValue(BackgroundBrushProperty, value); }
        }

        // Using a DependencyProperty as the backing store for BackgroundBrush.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty BackgroundBrushProperty =
            DependencyProperty.Register("BackgroundBrush", typeof(SolidColorBrush), typeof(PageHeader), new PropertyMetadata(new SolidColorBrush(Colors.White)));


        public CommandBar TopCommandBar
        {
            get
            {
                return topCommandBar;
            }
        }

        public Flyout ThemeFlyout
        {
            get
            {
                return themeFlyout;
            }
        }

        public PageHeader()
        {
            this.InitializeComponent();
        }

        private async void controlsSearchBox_TextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs args)
        {
            if (args.Reason == AutoSuggestionBoxTextChangeReason.UserInput)
            {
                var groups = await AppUIBasics.Data.ControlInfoDataSource.GetGroupsAsync();
                var suggestions = new List<ControlInfoDataItem>();

                foreach (var group in groups)
                {
                    var matchingItems = group.Items.Where(
                        item => item.Title.IndexOf(sender.Text, StringComparison.CurrentCultureIgnoreCase) >= 0);

                    foreach (var item in matchingItems)
                    {
                        suggestions.Add(item);
                    }
                }
                if (suggestions.Count > 0)
                    controlsSearchBox.ItemsSource = suggestions.OrderByDescending(i => i.Title.StartsWith(sender.Text, StringComparison.CurrentCultureIgnoreCase)).ThenBy(i => i.Title);
                else
                    controlsSearchBox.ItemsSource = new string[] { "No results found" };
            }
        }

        private void controlsSearchBox_QuerySubmitted(AutoSuggestBox sender, AutoSuggestBoxQuerySubmittedEventArgs args)
        {
            if (args.ChosenSuggestion != null && args.ChosenSuggestion is ControlInfoDataItem)
            {
                var itemId = (args.ChosenSuggestion as ControlInfoDataItem).UniqueId;
                NavigationRootPage.RootFrame.Navigate(typeof(ItemPage), itemId);
            }
            else if (!string.IsNullOrEmpty(args.QueryText))
            {
                NavigationRootPage.RootFrame.Navigate(typeof(SearchResultsPage), args.QueryText);
            }
        }

        private void searchButton_Click(object sender, RoutedEventArgs e)
        {
            controlsSearchBox.Visibility = Visibility.Visible;
            bool isFocused = controlsSearchBox.Focus(FocusState.Programmatic);
            if (!isFocused)
            {
                controlsSearchBox.UpdateLayout();
                controlsSearchBox.Focus(FocusState.Programmatic);
            }
            searchButton.Visibility = Visibility.Collapsed;
            commandBarBorder.Visibility = Visibility.Collapsed;
        }

        private void controlsSearchBox_LostFocus(object sender, RoutedEventArgs e)
        {
            if (Window.Current.Bounds.Width <= 640)
            {
                controlsSearchBox.Visibility = Visibility.Collapsed;
                commandBarBorder.Visibility = Visibility.Visible;
                searchButton.Visibility = Visibility.Visible;
            }
        }

        private void headerControl_Loaded(object sender, RoutedEventArgs e)
        {
            CoreApplicationViewTitleBar coreTitleBar = CoreApplication.GetCurrentView().TitleBar;
            if (NavigationRootPage.Current.DeviceFamily == DeviceType.Desktop)
            {
                MainTitleBar.Visibility = Visibility.Visible;
            }
            coreTitleBar.IsVisibleChanged += CoreTitleBar_IsVisibleChanged;
            this.Padding = NavigationRootPage.Current.DeviceFamily == DeviceType.Xbox ? new Thickness(24, 28, 48, 0) : NavigationRootPage.Current.DeviceFamily == DeviceType.Desktop ? new Thickness(24, 0, 12, 0) : new Thickness(14, 0, 14, 0);
        }

        private void CoreTitleBar_IsVisibleChanged(CoreApplicationViewTitleBar sender, object args)
        {
            MainTitleBar.Visibility = Visibility.Visible;

            this.Padding = NavigationRootPage.Current.DeviceFamily == DeviceType.Xbox ? new Thickness(24, 28, 48, 0) : NavigationRootPage.Current.DeviceFamily == DeviceType.Desktop ? new Thickness(24, 0, 12, 0) : new Thickness(14, 0, 14, 0);
        }

        private void ThemeButton_KeyDown(object sender, Windows.UI.Xaml.Input.KeyRoutedEventArgs e)
        {
            if (e.Key == Windows.System.VirtualKey.Right)
            {
                var nextElement = FocusManager.FindNextElement(FocusNavigationDirection.Right);
                if (nextElement == null)
                {
                    controlsSearchBox.Focus(FocusState.Programmatic);
                }
            }
        }
    }
}
