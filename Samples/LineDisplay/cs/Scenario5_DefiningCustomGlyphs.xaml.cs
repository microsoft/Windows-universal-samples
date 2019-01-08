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
using Windows.Security.Cryptography;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario5_DefiningCustomGlyphs : Page
    {
        private MainPage rootPage = MainPage.Current;
        private ClaimedLineDisplay lineDisplay;

        private IBuffer glyphBuffer;

        public Scenario5_DefiningCustomGlyphs()
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
            ResetButton.IsEnabled = false;
            SupportedGlyphsComboBox.Items.Clear();

            lineDisplay = await rootPage.ClaimScenarioLineDisplayAsync();

            if ((lineDisplay != null) && lineDisplay.Capabilities.CanDisplayCustomGlyphs)
            {
                string sampleText = "ABC";
                foreach (uint glyphCode in lineDisplay.CustomGlyphs.SupportedGlyphCodes)
                {
                    string glyphString = Char.ConvertFromUtf32((int)glyphCode);
                    SupportedGlyphsComboBox.Items.Add(new ComboBoxItem { Content = $"Code {glyphCode} (UTF-32 '{glyphString}')", Tag = glyphCode });
                    if (sampleText.Length < 10)
                    {
                        sampleText += glyphString;
                    }
                }

                if (SupportedGlyphsComboBox.Items.Count > 0)
                {
                    SupportedGlyphsComboBox.SelectedIndex = 0;
                }

                Size glyphSize = lineDisplay.CustomGlyphs.SizeInPixels;
                glyphBuffer = CreateSolidGlyphBuffer((int)glyphSize.Width, (int)glyphSize.Height);

                await lineDisplay.DefaultWindow.TryDisplayTextAsync(sampleText);
            }

            ResetButton.IsEnabled = true;
        }

        private async void DefineGlyphButton_Click(object sender, RoutedEventArgs e)
        {
            var selectedGlyphCode = Helpers.GetSelectedItemTag<uint>(SupportedGlyphsComboBox);
            if (await lineDisplay.CustomGlyphs.TryRedefineAsync((uint)selectedGlyphCode, glyphBuffer) &&
                await lineDisplay.DefaultWindow.TryRefreshAsync())
            {
                // Success. See results on the line display.
                rootPage.NotifyUser($"Redefined glyph {selectedGlyphCode}.", NotifyType.ErrorMessage);

            }
            else
            {
                rootPage.NotifyUser("Unable to redefine glyph and refresh.", NotifyType.ErrorMessage);
            }
        }

        private async void ResetButton_Click(object sender, RoutedEventArgs e)
        {
            lineDisplay?.Dispose();
            await InitializeAsync();
        }

        private IBuffer CreateSolidGlyphBuffer(int widthInPixels, int heightInPixels)
        {
            // Data is sent in one byte chunks, so find the minimum number of whole bytes 
            // needed to define each row of the glyph.
            int bytesPerRow = (widthInPixels + 7) / 8;

            byte[] newGlyphBuffer = new byte[bytesPerRow * heightInPixels];
            for (int i = 0; i < newGlyphBuffer.Length; i++)
            {
                // Set every bit so that we draw a solid rectangle.
                newGlyphBuffer[i] = 0xFF;
            }

            return CryptographicBuffer.CreateFromByteArray(newGlyphBuffer);
        }

        // Helpers that are used by XAML binding to enable and disable buttons.
        public bool IsNonNull(object item) => item != null;
    }
}
