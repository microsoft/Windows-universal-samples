﻿//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Animation;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class DetailsPage : Page
    {
        private Contact SelectedContact { set; get; }

        public DetailsPage()
        {
            InitializeComponent();
        }
        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            if (ShouldGoToWideState())
            {
                // We shouldn't see this page since we are in "wide master-detail" mode.
                // Play a transition as we are navigating from a separate page.
                NavigateBackForWideState(useTransition: true);
            }
            else
            {
                // Realize the main page content.
                FindName("RootPanel");
            }

            Window.Current.SizeChanged += OnWindowSizeChanged;
        }
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            SelectedContact = e.Parameter as Contact;
            // Register for hardware and software back request from the system
            SystemNavigationManager systemNavigationManager = SystemNavigationManager.GetForCurrentView();
            systemNavigationManager.BackRequested += OnBackRequested;
            systemNavigationManager.AppViewBackButtonVisibility = AppViewBackButtonVisibility.Visible;
        }
        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            SystemNavigationManager systemNavigationManager = SystemNavigationManager.GetForCurrentView();
            systemNavigationManager.BackRequested -= OnBackRequested;
            systemNavigationManager.AppViewBackButtonVisibility = AppViewBackButtonVisibility.Collapsed;
        }
        private void OnBackRequested(object sender, BackRequestedEventArgs e)
        {
            // Mark event as handled so we don't get bounced out of the app.
            e.Handled = true;
            // Page above us will be our master view.
            // Make sure we are using the "drill out" animation in this transition.
            Frame.Navigate(typeof(Scenario3_MasterDetail), "S" + SelectedContact.Id, new EntranceNavigationTransitionInfo());
        }
        void NavigateBackForWideState(bool useTransition)
        {
            // Evict this page from the cache as we may not need it again.
            NavigationCacheMode = NavigationCacheMode.Disabled;

            if (useTransition)
            {
                Frame.GoBack(new EntranceNavigationTransitionInfo());
            }
            else
            {
                Frame.GoBack(new SuppressNavigationTransitionInfo());
            }
        }
        private bool ShouldGoToWideState()
        {
            return Window.Current.Bounds.Width >= 720;
        }
        private void OnUnloaded(object sender, RoutedEventArgs e)
        {
            Window.Current.SizeChanged -= OnWindowSizeChanged;
        }
        private void OnWindowSizeChanged(object sender, WindowSizeChangedEventArgs e)
        {
            if (ShouldGoToWideState())
            {
                // Make sure we are no longer listening to window change events.
                Window.Current.SizeChanged -= OnWindowSizeChanged;

                // We shouldn't see this page since we are in "wide master-detail" mode.
                NavigateBackForWideState(useTransition: false);
            }
        }
        private void DeleteItem(object sender, RoutedEventArgs e)
        {
            Frame.Navigate(typeof(Scenario3_MasterDetail), "D" + SelectedContact.Id, new EntranceNavigationTransitionInfo());
        }
    }
}
