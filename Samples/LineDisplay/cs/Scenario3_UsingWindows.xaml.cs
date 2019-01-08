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
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using Windows.Devices.PointOfService;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public class WindowInfo
    {
        public LineDisplayWindow Window { get; private set; }
        public int Id { get; private set; }
        public string Name => (Id == 0) ? "(Default)" : String.Empty;

        public WindowInfo(LineDisplayWindow window, int id)
        {
            Window = window;
            Id = id;
        }
    }

    public sealed partial class Scenario3_UsingWindows : Page
    {
        private MainPage rootPage = MainPage.Current;
        private ClaimedLineDisplay lineDisplay;

        public ObservableCollection<WindowInfo> WindowList { get; } = new ObservableCollection<WindowInfo>();

        private int nextWindowId = 0;
        private uint maxWindows = 0;

        public Scenario3_UsingWindows()
        {
            this.InitializeComponent();
            DataContext = this;
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            await InitializeAsync();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            lineDisplay?.Dispose();
            lineDisplay = null;
        }

        private async Task InitializeAsync()
        {
            lineDisplay = await rootPage.ClaimScenarioLineDisplayAsync();

            maxWindows = (lineDisplay == null) ? 0 : lineDisplay.Capabilities.SupportedWindows;
            if (maxWindows >= 1)
            {
                // Create an entry to represent the default window.
                WindowList.Add(new WindowInfo(lineDisplay.DefaultWindow, nextWindowId));
                nextWindowId++;
            }
        }

        private async void NewWindowButton_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            Rect viewportBounds = new Rect()
            {
                X = Helpers.ParseUIntWithFallback(NewViewportX, -1.0),
                Y = Helpers.ParseUIntWithFallback(NewViewportY, -1.0),
                Width = Helpers.ParseUIntWithFallback(NewViewportWidth, 0.0),
                Height = Helpers.ParseUIntWithFallback(NewViewportHeight, 0.0),
            };

            if ((viewportBounds.Width <= 0) || (viewportBounds.Height <= 0))
            {
                rootPage.NotifyUser("Viewport size must be be positive.", NotifyType.ErrorMessage);
                return;
            }

            // Viewport cannot extend beyond the screen.
            Size screenSizeInCharacters = lineDisplay.GetAttributes().ScreenSizeInCharacters;
            if ((viewportBounds.X < 0) || (viewportBounds.Y < 0) ||
                (viewportBounds.X + viewportBounds.Width > screenSizeInCharacters.Width) ||
                (viewportBounds.Y + viewportBounds.Height > screenSizeInCharacters.Height))
            {
                rootPage.NotifyUser("Viewport cannot extend beyond the screen.", NotifyType.ErrorMessage);
                return;
            }

            Size windowSize = new Size()
            {
                Width = Helpers.ParseUIntWithFallback(NewWindowWidth, 0.0),
                Height = Helpers.ParseUIntWithFallback(NewWindowHeight, 0.0),
            };

            if ((windowSize.Width <= 0) || (windowSize.Height <= 0))
            {
                rootPage.NotifyUser("Window size must be be positive.", NotifyType.ErrorMessage);
                return;
            }

            // Windows must be at least as large as their viewports.
            if ((viewportBounds.Width > windowSize.Width) || (viewportBounds.Height > windowSize.Height))
            {
                rootPage.NotifyUser("Window must be at least as large as viewport.", NotifyType.ErrorMessage);
                return;
            }

            // Window taller than viewport requires IsVerticalMarqueeSupported.
            if ((windowSize.Height > viewportBounds.Height) && !lineDisplay.Capabilities.IsVerticalMarqueeSupported)
            {
                rootPage.NotifyUser("Window cannot be taller than viewport on this line display.", NotifyType.ErrorMessage);
                return;
            }

            // Window wider than viewport requires IsHorizontalMarqueeSupported.
            if ((windowSize.Width > viewportBounds.Width) && !lineDisplay.Capabilities.IsHorizontalMarqueeSupported)
            {
                rootPage.NotifyUser("Window cannot be wider than viewport on this line display.", NotifyType.ErrorMessage);
                return;
            }

            // Window cannot be larger than viewport in both directions.
            // (Two-dimensional scrolling is not supported.)
            if ((windowSize.Width > viewportBounds.Width) && (windowSize.Height > viewportBounds.Height))
            {
                rootPage.NotifyUser("Window cannot be larger than viewport in both dimensions.", NotifyType.ErrorMessage);
                return;
            }

            LineDisplayWindow newWindow = await lineDisplay.TryCreateWindowAsync(viewportBounds, windowSize);

            if (newWindow != null)
            {
                var newWindowInfo = new WindowInfo(newWindow, nextWindowId);
                WindowList.Add(newWindowInfo);
                nextWindowId++;

                rootPage.NotifyUser($"Created window {newWindowInfo.Id}.", NotifyType.StatusMessage);
            }
            else
            {
                // We probably lost our claim.
                rootPage.NotifyUser("Failed to create a new window", NotifyType.ErrorMessage);
            }
        }

        private async void RefreshWindowButton_Click(object sender, RoutedEventArgs e)
        {
            var targetWindow = (WindowInfo)WindowsListBox.SelectedItem;
            if (await targetWindow.Window.TryRefreshAsync())
            {
                rootPage.NotifyUser($"Refreshed window {targetWindow.Id}.", NotifyType.StatusMessage);
            }
            else
            {
                // We probably lost our claim.
                rootPage.NotifyUser($"Failed to refresh window {targetWindow.Id}.", NotifyType.ErrorMessage);
            }
        }

        private void DestroyWindowButton_Click(object sender, RoutedEventArgs e)
        {
            var targetWindow = (WindowInfo)WindowsListBox.SelectedItem;
            WindowList.RemoveAt(WindowsListBox.SelectedIndex);

            // The LineDisplayWindow class implements IDispose. Disposing of a LineDisplayWindow
            // results in its destruction on the line display device and invalidation of the object.
            targetWindow.Window.Dispose();

            rootPage.NotifyUser($"Destroyed window {targetWindow.Id}.", NotifyType.StatusMessage);
        }

        private async void DisplayTextButton_Click(object sender, RoutedEventArgs e)
        {
            var targetWindow = (WindowInfo)WindowsListBox.SelectedItem;
            if (await targetWindow.Window.TryDisplayTextAsync(DisplayTextTextbox.Text))
            {
                rootPage.NotifyUser($"Displayed \"{DisplayTextTextbox.Text}\" to window {targetWindow.Id}.", NotifyType.StatusMessage);
            }
            else
            {
                // We probably lost our claim.
                rootPage.NotifyUser($"Failed to display to window {targetWindow.Id}.", NotifyType.ErrorMessage);
            }
        }

        // Helpers that are used by XAML binding to enable and disable buttons.

        // The default window cannot be destroyed.
        public bool CanDestroyWindow(object item) => (item != null) && (((WindowInfo)item).Id != 0);

        // Can create a new window if we haven't reached the maximum.
        public bool CanCreateNewWindow(ObservableCollection<WindowInfo> windowsList) => windowsList.Count < maxWindows;

        // Some buttons require a selection.
        public bool IsNonNull(object item) => item != null;
    }
}
