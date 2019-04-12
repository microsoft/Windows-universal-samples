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
using Windows.UI.ViewManagement;
using Windows.UI.WindowManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Hosting;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario4_CompactOverlay : Page
    {
        private AppWindow appWindow;
        private Frame appWindowFrame = new Frame();

        public Scenario4_CompactOverlay()
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
            // Is CompactOverlay supported for our main window?
            // If so, it will be supported for a new window as well.
            // If it isn't, it will not be supported for new windows either so we cannot proceed.
            if (!ApplicationView.GetForCurrentView().IsViewModeSupported(ApplicationViewMode.CompactOverlay))
            {
                return;
            }

            showWindowBtn.IsEnabled = false;

            if (appWindow == null)
            {

                // Create a new AppWindow
                appWindow = await AppWindow.TryCreateAsync();
                // Make sure we release the reference to this window, and release XAML resources, when it's closed
                appWindow.Closed += delegate { appWindow = null; appWindowFrame.Content = null; };
                // Is CompactOverlay supported for this AppWindow? If not, then stop.
                if (appWindow.Presenter.IsPresentationSupported(AppWindowPresentationKind.CompactOverlay))
                {
                    // Create a new frame for the window
                    // Navigate the frame to the CompactOverlay page inside it.
                    appWindowFrame.Navigate(typeof(SecondaryAppWindowPage));
                    // Attach the frame to the window
                    ElementCompositionPreview.SetAppWindowContent(appWindow, appWindowFrame);
                    // Let's set the title so that we can tell the windows apart
                    appWindow.Title = "CompactOverlay Window";
                    // Request the Presentation of the window to CompactOverlay
                    bool switched = appWindow.Presenter.RequestPresentation(AppWindowPresentationKind.CompactOverlay);
                    if (switched)
                    {
                        // If the request was satisfied, show the window
                        await appWindow.TryShowAsync();
                    }
                }
            }
            else
            {
                await appWindow.TryShowAsync();
            }

            showWindowBtn.IsEnabled = true;
        }
    }
}
