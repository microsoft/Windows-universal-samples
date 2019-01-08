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

using System.Collections.Generic;
using Windows.Globalization;
using Windows.Media.Ocr;
using Windows.Storage;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1_Configuration : Page
    {
        // A pointer back to the main page.
        // This is needed if you want to call methods in MainPage such as NotifyUser()
        private MainPage rootPage = MainPage.Current;

        private static ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;

        public Scenario1_Configuration()
        {
            this.InitializeComponent();

            IReadOnlyList<Language> availableLanguages = OcrEngine.AvailableRecognizerLanguages;

            LanguageList.ItemsSource = availableLanguages;

            if (availableLanguages.Count > 0)
            {
                // Select the one that the user selected last time, if any.
                if (localSettings.Values["decoderLanguage"] is string languageTag)
                {
                    foreach (Language language in availableLanguages)
                    {
                        if (language.LanguageTag == languageTag)
                        {
                            LanguageList.SelectedItem = language;
                            break;
                        }
                    }
                }

                // If not found (or user had never set a preference), then select the first one.
                if (LanguageList.SelectedIndex < 0)
                {
                    LanguageList.SelectedIndex = 0;
                }
            }
        }

        /// <summary>
        /// Occurs when selected language is changed in available languages combo box.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void LanguageList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var selectedLanguage = LanguageList.SelectedItem as Language;
            if (selectedLanguage != null)
            {
                rootPage.NotifyUser($"Selected decoder language is {selectedLanguage.DisplayName}.", NotifyType.StatusMessage);

                localSettings.Values["decoderLanguage"] = selectedLanguage.LanguageTag;
            }
        }
    }
}
