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
using Windows.UI.WindowManagement;
using Windows.UI.WindowManagement.Preview;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Hosting;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario3_Size : Page
    {
        private AppWindow appWindow;
        private Frame appWindowFrame = new Frame();

        public Scenario3_Size()
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

        // Lower bounds on window size to keep things from being unresonably tiny.
        const double MinWindowWidth = 192;
        const double MinWindowHeight = 48;

        private async void ShowWindowBtn_Click(object sender, RoutedEventArgs e)
        {
            // Clear any previous message.
            MainPage.Current.NotifyUser(string.Empty, NotifyType.StatusMessage);

            Double.TryParse(windowWidthTxt.Text, out var windowWidth);
            Double.TryParse(windowHeightTxt.Text, out var windowHeight);
            if (windowWidth < MinWindowWidth || windowHeight < MinWindowHeight)
            {
                MainPage.Current.NotifyUser($"Please specify a width of at least {MinWindowWidth} and a height of at least {MinWindowHeight}.", NotifyType.ErrorMessage);
                return;
            }

            showWindowBtn.IsEnabled = false;

            // Only ever create and show one window. If the AppWindow exists use it
            if (appWindow == null)
            {
                // Create a new window
                appWindow = await AppWindow.TryCreateAsync();
                // Make sure we release the reference to this window, and release XAML resources, when it's closed
                appWindow.Closed += delegate { appWindow = null; appWindowFrame.Content = null; };
                // Navigate the frame to the page we want to show in the new window
                appWindowFrame.Navigate(typeof(SecondaryAppWindowPage));
            }

            // If specified size is smaller than the default min size for a window we need to set a new preferred min size first.
            // Let's set it to the smallest allowed and leave it at that.
            if (windowWidth < 500 || windowHeight < 320)
            {
                WindowManagementPreview.SetPreferredMinSize(appWindow, new Size(MinWindowWidth, MinWindowHeight));
            }
            // Request the size of our window
            appWindow.RequestSize(new Size(windowWidth, windowHeight));
            // Attach the XAML content to our window
            ElementCompositionPreview.SetAppWindowContent(appWindow, appWindowFrame);

            // If the window is not visible, show it and/or bring it to foreground
            if (!appWindow.IsVisible)
            {
                await appWindow.TryShowAsync();
            }

            showWindowBtn.IsEnabled = true;
        }
    }
}
