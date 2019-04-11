//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Collections.Generic;
using Windows.UI.ViewManagement;
using Windows.UI.WindowManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Hosting;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario2_DisplayRegion : Page
    {
        private AppWindow appWindow;
        private Frame appWindowFrame = new Frame();

        public Scenario2_DisplayRegion()
        {
            InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Check to see if we have more than one DisplayRegion, if not we disable the button since the scenario will not work
            showWindowBtn.IsEnabled = NumberOfAvailableDisplayRegions() > 1;
        }

        protected override async void OnNavigatedFrom(NavigationEventArgs e)
        {
            if (appWindow != null)
            {
                await appWindow.CloseAsync();
            }
        }

        private int NumberOfAvailableDisplayRegions()
        {
            // Get the list of all DisplayRegions defined for the WindowingEnvironment that our application is currently in
            IReadOnlyList<DisplayRegion> displayRegions = ApplicationView.GetForCurrentView().WindowingEnvironment.GetDisplayRegions();

            int availableDisplayRegions = 0;
            // count all the DisplayRegions that are marked as available to the app for use.
            foreach (DisplayRegion displayregion in displayRegions)
            {
                if (displayregion.IsVisible)
                {
                    availableDisplayRegions++;
                }
            }
            return availableDisplayRegions;
        }

        private DisplayRegion GetOtherDisplayRegion(DisplayRegion currentAppDisplayRegion)
        {
            // Get the list of all DisplayRegions defined for the WindowingEnvironment that our application is currently in
            IReadOnlyList<DisplayRegion> displayRegions = ApplicationView.GetForCurrentView().WindowingEnvironment.GetDisplayRegions();
            foreach (DisplayRegion displayRegion in displayRegions)
            {
                if (displayRegion != currentAppDisplayRegion && displayRegion.IsVisible)
                {
                    return displayRegion;
                }
            }

            return null;
        }

        private async void Button_Click(object sender, RoutedEventArgs e)
        {
            showWindowBtn.IsEnabled = false;

            // Only ever create and show one window. If the AppWindow exists call TryShow on it to bring it to foreground.
            if (appWindow == null)
            {
                // Create a new window
                appWindow = await AppWindow.TryCreateAsync();
                // Make sure we release the reference to this window, and release XAML resources, when it's closed
                appWindow.Closed += delegate { appWindow = null; appWindowFrame.Content = null; };
                // Navigate the frame to the page we want to show in the new window
                appWindowFrame.Navigate(typeof(SecondaryAppWindowPage));
                // Attach the XAML content to our window
                ElementCompositionPreview.SetAppWindowContent(appWindow, appWindowFrame);
            }

            // Get DisplayRegion to position our window on
            DisplayRegion secondaryDisplayRegion = GetOtherDisplayRegion(ApplicationView.GetForCurrentView().GetDisplayRegions()[0]);
            if (secondaryDisplayRegion != null)
            {
                appWindow.RequestMoveToDisplayRegion(secondaryDisplayRegion);
            }

            // If the window is not visible, show it and/or bring it to foreground
            if (!appWindow.IsVisible)
            {
                await appWindow.TryShowAsync();
            }

            showWindowBtn.IsEnabled = true;
        }
    }
}
