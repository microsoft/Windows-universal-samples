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
using Windows.Foundation;
using Windows.UI.ViewManagement;
using Windows.UI.WindowManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Hosting;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario5_RelativePositioning : Page
    {
        private AppWindow appWindow;
        private Frame appWindowFrame = new Frame();

        public Scenario5_RelativePositioning()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedFrom(NavigationEventArgs e)
        {
            if (appWindow != null)
            {
                await appWindow.CloseAsync();
            }
        }

        private async void ShowWindowBtn_Click(object sender, RoutedEventArgs e)
        {
            // Clear any previous message.
            MainPage.Current.NotifyUser(string.Empty, NotifyType.StatusMessage);

            if (!Double.TryParse(windowOffsetXTxt.Text, out var horizontalOffset) ||
                !Double.TryParse(windowOffsetYTxt.Text, out var verticalOffset))
            {
                MainPage.Current.NotifyUser($"Please specify valid numeric offsets.", NotifyType.ErrorMessage);
                return;
            }

            showWindowBtn.IsEnabled = false;

            // Only ever create and show one window.
            if (appWindow == null)
            {
                // Create a new window
                appWindow = await AppWindow.TryCreateAsync();
                // Make sure we release the reference to this window, and release XAML resources, when it's closed
                appWindow.Closed += delegate { appWindow = null; appWindowFrame.Content = null; };
                // Request the size of our window
                appWindow.RequestSize(new Size(500, 320));
                // Navigate the frame to the page we want to show in the new window
                appWindowFrame.Navigate(typeof(SecondaryAppWindowPage));
                // Attach the XAML content to our window
                ElementCompositionPreview.SetAppWindowContent(appWindow, appWindowFrame);
            }

            // Make a point for our offset.
            Point offset = new Point(horizontalOffset, verticalOffset);

            // Check if we should be setting our position relative to ApplicationView or DisplayRegion
            if (positionOffsetDisplayRegionRB.IsChecked.Value)
            {
                // Request an offset relative to the DisplayRegion we're on
                appWindow.RequestMoveRelativeToDisplayRegion(ApplicationView.GetForCurrentView().GetDisplayRegions()[0], offset);
            }
            else
            {
                // Relative to our ApplicationView
                appWindow.RequestMoveRelativeToCurrentViewContent(offset);
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