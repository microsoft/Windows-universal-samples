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

using Windows.Foundation;
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

        public Scenario1_Basic()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            Window.Current.SizeChanged += OnWindowSizeChanged;
            UpdateContent();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            Window.Current.SizeChanged -= OnWindowSizeChanged;
        }

        private void ResizeView_Click(object sender, RoutedEventArgs e)
        {
            var view = ApplicationView.GetForCurrentView();
            if (view.TryResizeView(new Size { Width = 600, Height = 500 }))
            {
                rootPage.NotifyUser("Resizing to 600 \u00D7 500", NotifyType.StatusMessage);
                // The SizeChanged event will be raised when the resize is complete.
            }
            else
            {
                // The system does not support resizing, or the provided size is out of range.
                rootPage.NotifyUser("Failed to resize view", NotifyType.ErrorMessage);
            }
        }

        private void SetMinimumSize_Click(object sender, RoutedEventArgs e)
        {
            var view = ApplicationView.GetForCurrentView();
            if (SetMinimumSize.IsChecked.Value)
            {
                // If this size is not permitted by the system, the nearest permitted value is used.
                view.SetPreferredMinSize(new Size { Width = 300, Height = 200 });
            }
            else
            {
                // Passing width = height = 0 returns to the default system behavior.
                view.SetPreferredMinSize(new Size());
            }
        }

        void OnWindowSizeChanged(object sender, WindowSizeChangedEventArgs e)
        {
            UpdateContent();
        }

        void UpdateContent()
        {
            Rect bounds = Window.Current.Bounds;
            ReportViewWidth.Text = bounds.Width.ToString();
            ReportViewHeight.Text = bounds.Height.ToString();
        }
    }
}
