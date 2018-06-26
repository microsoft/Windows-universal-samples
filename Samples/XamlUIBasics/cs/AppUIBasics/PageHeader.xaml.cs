//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using AppUIBasics.Data;
using System;
using System.Collections.Generic;
using System.Linq;
using Windows.System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

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


        public CommandBar TopCommandBar
        {
            get { return topCommandBar; }
        }

        public UIElement TitlePanel
        {
            get { return pageTitle; }
        }

        public Action ToggleThemeAction { get; set; }

        public PageHeader()
        {
            this.InitializeComponent();
        }

        public void UpdateBackground(bool isFilteredPage)
        {
            VisualStateManager.GoToState(this, isFilteredPage ? "FilteredPage" : "NonFilteredPage", false);
        }

        private void OnControlsSearchBoxTextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs args)
        {
            if (args.Reason == AutoSuggestionBoxTextChangeReason.UserInput)
            {
                var suggestions = new List<ControlInfoDataItem>();

                foreach (var group in ControlInfoDataSource.Instance.Groups)
                {
                    var matchingItems = group.Items.Where(
                        item => item.Title.IndexOf(sender.Text, StringComparison.CurrentCultureIgnoreCase) >= 0);

                    foreach (var item in matchingItems)
                    {
                        suggestions.Add(item);
                    }
                }
                if (suggestions.Count > 0)
                {
                    controlsSearchBox.ItemsSource = suggestions.OrderByDescending(i => i.Title.StartsWith(sender.Text, StringComparison.CurrentCultureIgnoreCase)).ThenBy(i => i.Title);
                }
                else
                {
                    controlsSearchBox.ItemsSource = new string[] { "No results found" };
                }
            }
        }

        private void OnControlsSearchBoxQuerySubmitted(AutoSuggestBox sender, AutoSuggestBoxQuerySubmittedEventArgs args)
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

        private void OnSearchButtonClick(object sender, RoutedEventArgs e)
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

        private void OnControlsSearchBoxLostFocus(object sender, RoutedEventArgs e)
        {
            if (Window.Current.Bounds.Width <= 640)
            {
                controlsSearchBox.Visibility = Visibility.Collapsed;
                commandBarBorder.Visibility = Visibility.Visible;
                searchButton.Visibility = Visibility.Visible;
            }
        }

        private void OnThemeButtonKeyDown(object sender, KeyRoutedEventArgs e)
        {
            if (e.Key == VirtualKey.Right)
            {
                var nextElement = FocusManager.FindNextElement(FocusNavigationDirection.Right);
                if (nextElement == null)
                {
                    controlsSearchBox.Focus(FocusState.Programmatic);
                }
            }
        }

        private void OnThemeButtonClick(object sender, RoutedEventArgs e)
        {
            ToggleThemeAction?.Invoke();
        }

        private void KeyboardAccelerator_Invoked(KeyboardAccelerator sender, KeyboardAcceleratorInvokedEventArgs args)
        {
            controlsSearchBox.Focus(FocusState.Keyboard);
        }
    }
}