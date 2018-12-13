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
using System.Threading.Tasks;
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
        private ClaimedLineDisplay lineDisplay;

        public Scenario2_DisplayText()
        {
            this.InitializeComponent();
        }

        protected async override void OnNavigatedTo(NavigationEventArgs e)
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
            BlinkCheckBox.IsEnabled = (lineDisplay != null) && (lineDisplay.Capabilities.CanBlink != LineDisplayTextAttributeGranularity.NotSupported);
            DisplayTextButton.IsEnabled = (lineDisplay != null);
        }

        private async void DisplayTextButton_Click(object sender, RoutedEventArgs e)
        {
            string text = "Hello from UWP";

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

            if (BlinkCheckBox.IsChecked.Value)
            {
                attribute = LineDisplayTextAttribute.Blink;
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
    }
}
