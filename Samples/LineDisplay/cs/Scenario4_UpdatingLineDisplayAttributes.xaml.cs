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
    public sealed partial class Scenario4_UpdatingLineDisplayAttributes : Page
    {
        private MainPage rootPage = MainPage.Current;
        private ClaimedLineDisplay lineDisplay;

        public Scenario4_UpdatingLineDisplayAttributes()
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

            BlinkRateSlider.IsEnabled = (lineDisplay != null) && lineDisplay.Capabilities.CanChangeBlinkRate;
            BrightnessSlider.IsEnabled = (lineDisplay != null) && lineDisplay.Capabilities.IsBrightnessSupported;

            SupportedScreenSizesComboBox.IsEnabled = (lineDisplay != null) && lineDisplay.Capabilities.CanChangeScreenSize;

            if (SupportedScreenSizesComboBox.IsEnabled)
            {
                foreach (Size screenSize in lineDisplay.SupportedScreenSizesInCharacters)
                {
                    SupportedScreenSizesComboBox.Items.Add(new ComboBoxItem { Content = $"{screenSize.Width} x {screenSize.Height}", Tag = screenSize });
                }

                if (SupportedScreenSizesComboBox.Items.Count > 0)
                {
                    SupportedScreenSizesComboBox.SelectedIndex = 0;
                }
            }

            CharacterSetMappingEnabledCheckbox.IsEnabled = (lineDisplay != null) && lineDisplay.Capabilities.CanMapCharacterSets;

            UpdateButton.IsEnabled = (lineDisplay != null);

            if (UpdateButton.IsEnabled)
            {
                SetValuesFromLineDisplay();

                if (lineDisplay.Capabilities.CanBlink == LineDisplayTextAttributeGranularity.NotSupported)
                {
                    await lineDisplay.DefaultWindow.TryDisplayTextAsync("Regular Text");
                }
                else
                {
                    await lineDisplay.DefaultWindow.TryDisplayTextAsync("Blinking Text", LineDisplayTextAttribute.Blink);
                }
            }
        }

        private void SetValuesFromLineDisplay()
        {
            LineDisplayAttributes attributes = lineDisplay.GetAttributes();
            BlinkRateSlider.Value = attributes.BlinkRate.TotalMilliseconds;
            BrightnessSlider.Value = attributes.Brightness;
            CharacterSetMappingEnabledCheckbox.IsChecked = attributes.IsCharacterSetMappingEnabled;
        }

        private async void UpdateButton_Click(object sender, RoutedEventArgs e)
        {
            LineDisplayAttributes attributes = lineDisplay.GetAttributes();

            if (lineDisplay.Capabilities.CanChangeBlinkRate)
            {
                attributes.BlinkRate = TimeSpan.FromMilliseconds(BlinkRateSlider.Value);
            }

            if (lineDisplay.Capabilities.IsBrightnessSupported)
            {
                attributes.Brightness = (int)BrightnessSlider.Value;
            }

            if (lineDisplay.Capabilities.CanChangeScreenSize)
            {
                attributes.ScreenSizeInCharacters = Helpers.GetSelectedItemTag<Size>(SupportedScreenSizesComboBox);
            }

            if (lineDisplay.Capabilities.CanMapCharacterSets)
            {
                attributes.IsCharacterSetMappingEnabled = CharacterSetMappingEnabledCheckbox.IsChecked.Value;
            }

            if (await lineDisplay.TryUpdateAttributesAsync(attributes))
            {
                rootPage.NotifyUser("Attributes updated successfully.", NotifyType.StatusMessage);

                // The resulting attributes may not match our request.
                // For example, the Line Display will choose the closest available blink rate.
                // Update the controls to show what the Line Display actually used.
                SetValuesFromLineDisplay();
            }
            else
            {
                // We probably lost our claim.
                rootPage.NotifyUser("Failed to update attributes.", NotifyType.ErrorMessage);
            }
        }
    }
}
