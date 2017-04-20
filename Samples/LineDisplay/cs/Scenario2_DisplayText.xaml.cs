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
using Windows.Devices.PointOfService;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario2_DisplayText : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario2_DisplayText()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (String.IsNullOrEmpty(rootPage.LineDisplayId))
            {
                rootPage.NotifyUser("You must use scenario 1 to select a line display", NotifyType.ErrorMessage);
                DisplayTextButton.IsEnabled = false;
            }
        }

        private async void DisplayTextButton_Click(object sender, RoutedEventArgs e)
        {
            string text = "Hello from UWP";

            using (ClaimedLineDisplay lineDisplay = await ClaimedLineDisplay.FromIdAsync(rootPage.LineDisplayId))
            {
                if (lineDisplay != null)
                {
                    var position = new Point(0, 0);
                    if (CenterCheckBox.IsChecked.Value)
                    {
                        var length = text.Length;
                        if (length < lineDisplay.DefaultWindow.SizeInCharacters.Width)
                        {
                            position.X = ((int)lineDisplay.DefaultWindow.SizeInCharacters.Width - length) / 2;
                        }
                    }

                    LineDisplayTextAttribute attribute = LineDisplayTextAttribute.Normal;

                    // If blinking is requested, verify that the device supports blinking.
                    if (BlinkCheckBox.IsChecked.Value)
                    {
                        if (lineDisplay.Capabilities.CanBlink == LineDisplayTextAttributeGranularity.NotSupported)
                        {
                            BlinkNotSupportedText.Visibility = Visibility.Visible;
                        }
                        else
                        {
                            // Device supports blinking.
                            attribute = LineDisplayTextAttribute.Blink;
                        }
                    }

                    if (await lineDisplay.DefaultWindow.TryClearTextAsync() &&
                        await lineDisplay.DefaultWindow.TryDisplayTextAsync(text, attribute, position))
                    {
                        rootPage.NotifyUser("Text displayed successfully", NotifyType.StatusMessage);
                    }
                    else
                    {
                        // We probably lost our claim.
                        rootPage.NotifyUser("Unable to display text", NotifyType.ErrorMessage);
                    }
                }
                else
                {
                    rootPage.NotifyUser("Unable to claim the Line Display", NotifyType.ErrorMessage);
                }
            }
        }
    }
}
