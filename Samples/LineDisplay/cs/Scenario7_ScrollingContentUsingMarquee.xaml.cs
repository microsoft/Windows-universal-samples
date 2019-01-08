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
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Threading.Tasks;
using Windows.Devices.PointOfService;
using Windows.Foundation;
using Windows.Security.Cryptography;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario7_ScrollingContentUsingMarquee : Page
    {
        private MainPage rootPage = MainPage.Current;
        private ClaimedLineDisplay lineDisplay;
        private LineDisplayWindow horizontalScrollableWindow;
        private LineDisplayWindow verticalScrollableWindow;

        public Scenario7_ScrollingContentUsingMarquee()
        {
            this.InitializeComponent();
            this.DataContext = this;
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            await InitializeAsync();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            horizontalScrollableWindow?.Dispose();
            horizontalScrollableWindow = null;
            verticalScrollableWindow?.Dispose();
            verticalScrollableWindow = null;
            lineDisplay?.Dispose();
            lineDisplay = null;
        }

        private async Task InitializeAsync()
        {
            lineDisplay = await rootPage.ClaimScenarioLineDisplayAsync();

            if (lineDisplay != null)
            {
                Size screenSize = lineDisplay.GetAttributes().ScreenSizeInCharacters;
                int windowCount = 1;

                if (lineDisplay.Capabilities.IsHorizontalMarqueeSupported && (windowCount < lineDisplay.Capabilities.SupportedWindows))
                {
                    // Create a horizontal scrollable window by creating a window
                    // whose width is wider than the viewport.
                    horizontalScrollableWindow = await lineDisplay.TryCreateWindowAsync(
                        new Rect()
                        {
                            X = 0,
                            Y = 0,
                            Width = screenSize.Width,
                            Height = screenSize.Height
                        },
                        new Size()
                        {
                            Width = screenSize.Width * 2,
                            Height = screenSize.Height
                        });

                    if (horizontalScrollableWindow != null)
                    {
                        windowCount++;
                        ScrollDirectionComboBox.Items.Add(LineDisplayScrollDirection.Left);
                        ScrollDirectionComboBox.Items.Add(LineDisplayScrollDirection.Right);
                    }
                }

                if (lineDisplay.Capabilities.IsVerticalMarqueeSupported && (windowCount < lineDisplay.Capabilities.SupportedWindows))
                {
                    // Create a vertical scrollable window by creating a window
                    // whose height is taller than the viewport.
                    verticalScrollableWindow = await lineDisplay.TryCreateWindowAsync(
                        new Rect()
                        {
                            X = 0,
                            Y = 0,
                            Width = screenSize.Width,
                            Height = screenSize.Height
                        },
                        new Size()
                        {
                            Width = screenSize.Width,
                            Height = screenSize.Height * 2
                        });

                    if (verticalScrollableWindow != null)
                    {
                        windowCount++;
                        ScrollDirectionComboBox.Items.Add(LineDisplayScrollDirection.Up);
                        ScrollDirectionComboBox.Items.Add(LineDisplayScrollDirection.Down);
                    }
                }
            }

            if (ScrollDirectionComboBox.Items.Count > 0)
            {
                ScrollDirectionComboBox.SelectedIndex = 0;
                StartScrollingButton.IsEnabled = true;
                StopScrollingButton.IsEnabled = true;
            }
        }

        private async void StartScrollingButton_Click(object sender, RoutedEventArgs e)
        {
            LineDisplayScrollDirection scrollDirection = (LineDisplayScrollDirection)ScrollDirectionComboBox.SelectedItem;
            LineDisplayWindow selectedWindow = null;

            if (scrollDirection == LineDisplayScrollDirection.Left || scrollDirection == LineDisplayScrollDirection.Right)
            {
                selectedWindow = horizontalScrollableWindow;
            }
            else
            {
                selectedWindow = verticalScrollableWindow;
            }

            // Cannot make changes while scrolling is active.
            await StopAllScrollingAsync();

            // Bring the window to the front, clear its old contents,
            // and display some text that will require scrolling.
            await selectedWindow.TryRefreshAsync();
            await selectedWindow.TryClearTextAsync();
            await selectedWindow.TryDisplayTextAsync(
                "An incredibly lengthy string of arbitrary text that cannot be displayed on most line displays " +
                "without being scrolled into view. It will need to be scrolled into view left, right, up or down.");

            // Configure the scrolling behavior and start scrolling.
            selectedWindow.Marquee.Format = (LineDisplayMarqueeFormat)MarqueeFormatComboBox.SelectedItem;
            selectedWindow.Marquee.RepeatWaitInterval = TimeSpan.FromMilliseconds(RepeatWaitIntervalSlider.Value);
            selectedWindow.Marquee.ScrollWaitInterval = TimeSpan.FromMilliseconds(ScrollWaitIntervalSlider.Value);
            await selectedWindow.Marquee.TryStartScrollingAsync(scrollDirection);
        }

        private async Task StopAllScrollingAsync()
        {
            if (horizontalScrollableWindow != null)
            {
                await horizontalScrollableWindow.Marquee.TryStopScrollingAsync();
            }

            if (verticalScrollableWindow != null)
            {
                await verticalScrollableWindow.Marquee.TryStopScrollingAsync();
            }
        }
        private async void StopScrollingButton_Click(object sender, RoutedEventArgs e)
        {
            await StopAllScrollingAsync();
        }
    }
}
