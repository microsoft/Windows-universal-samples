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

using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Basic : Page
    {
        private MainPage rootPage;

        // What is the program's last known full-screen state?
        // We use this to detect when the system forced us out of full-screen mode.
        private bool isLastKnownFullScreen = ApplicationView.GetForCurrentView().IsFullScreenMode;

        public Scenario1_Basic()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            Window.Current.SizeChanged += OnWindowResize;
            rootPage.KeyDown += OnKeyDown;
            UpdateContent();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            Window.Current.SizeChanged -= OnWindowResize;
            rootPage.KeyDown -= OnKeyDown;
        }

        private void ToggleFullScreenModeButton_Click(object sender, RoutedEventArgs e)
        {
            var view = ApplicationView.GetForCurrentView();
            if (view.IsFullScreenMode)
            {
                view.ExitFullScreenMode();
                rootPage.NotifyUser("Exiting full screen mode", NotifyType.StatusMessage);
                isLastKnownFullScreen = false;
                // The SizeChanged event will be raised when the exit from full screen mode is complete.
            }
            else
            {
                if (view.TryEnterFullScreenMode())
                {
                    rootPage.NotifyUser("Entering full screen mode", NotifyType.StatusMessage);
                    isLastKnownFullScreen = true;
                    // The SizeChanged event will be raised when the entry to full screen mode is complete.
                }
                else
                {
                    rootPage.NotifyUser("Failed to enter full screen mode", NotifyType.ErrorMessage);
                }
            }
        }

        private void ShowStandardSystemOverlaysButton_Click(object sender, RoutedEventArgs e)
        {
            var view = ApplicationView.GetForCurrentView();
            view.ShowStandardSystemOverlays();
        }

        private void UseMinimalOverlaysCheckBox_Click(object sender, RoutedEventArgs e)
        {
            var view = ApplicationView.GetForCurrentView();
            view.FullScreenSystemOverlayMode = UseMinimalOverlaysCheckBox.IsChecked.Value ? FullScreenSystemOverlayMode.Minimal : FullScreenSystemOverlayMode.Standard;
        }

        void OnWindowResize(object sender, WindowSizeChangedEventArgs e)
        {
            UpdateContent();
        }

        void UpdateContent()
        {
            var view = ApplicationView.GetForCurrentView();
            var isFullScreenMode = view.IsFullScreenMode;
            ToggleFullScreenModeSymbol.Symbol = isFullScreenMode ? Symbol.BackToWindow : Symbol.FullScreen;
            ReportFullScreenMode.Text = isFullScreenMode ? "is in" : "is not in";
            FullScreenOptionsPanel.Visibility = isFullScreenMode ? Visibility.Visible : Visibility.Collapsed;

            // Did the system force a change in full screen mode?
            if (isLastKnownFullScreen != isFullScreenMode)
            {
                isLastKnownFullScreen = isFullScreenMode;
                // Clear any stray messages that talked about the mode we are no longer in.
                rootPage.NotifyUser("", NotifyType.StatusMessage);
            }
        }

        void OnKeyDown(object sender, KeyRoutedEventArgs e)
        {
            if (e.Key == Windows.System.VirtualKey.Escape)
            {
                var view = ApplicationView.GetForCurrentView();
                if (view.IsFullScreenMode)
                {
                    view.ExitFullScreenMode();
                    rootPage.NotifyUser("Exited full screen mode due to keypress", NotifyType.StatusMessage);
                    isLastKnownFullScreen = false;
                }
            }
        }
    }
}
