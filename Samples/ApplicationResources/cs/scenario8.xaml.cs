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
using System.Text;
using Windows.ApplicationModel.Resources.Core;
using Windows.Globalization;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario8 : Page
    {
        int lastSelectedIndex;

        public Scenario8()
        {
            this.InitializeComponent();
            PopulateComboBox();
            UpdateCurrentAppLanguageMessage();
        }

        private void ShowText()
        {
            ResourceContext defaultContextForCurrentView = ResourceContext.GetForCurrentView();
            ResourceMap stringResourcesResourceMap = ResourceManager.Current.MainResourceMap.GetSubtree("Resources");
            Scenario8MessageTextBlock.Text = stringResourcesResourceMap.GetValue("string1", defaultContextForCurrentView).ValueAsString;
        }

        private void LanguageOverrideCombo_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var combo = sender as ComboBox;
            var item = combo.SelectedValue as ComboBoxItem;
            var languageTag = item.Tag as string;

            // Ignore the divider (tag = "-")
            if (languageTag == "-")
            {
                combo.SelectedIndex = lastSelectedIndex;
            }
            else
            {
                lastSelectedIndex = combo.SelectedIndex;

                // Set the persistent language override
                ApplicationLanguages.PrimaryLanguageOverride = languageTag;

                // update current app languages message
                UpdateCurrentAppLanguageMessage();
            }
        }

        void AddItemForLanguageTag(string languageTag)
        {
            var language = new Language(languageTag);
            var item = new ComboBoxItem { Content = language.DisplayName, Tag = languageTag };
            LanguageOverrideCombo.Items.Add(item);

            // Select this item if it is the primary language override.
            if (languageTag == ApplicationLanguages.PrimaryLanguageOverride)
            {
                LanguageOverrideCombo.SelectedItem = item;
            }
        }

        void PopulateComboBox()
        {
            // First show the default setting
            LanguageOverrideCombo.Items.Add(new ComboBoxItem { Content = "Use language preferences (recommended)", Tag = "" });
            LanguageOverrideCombo.SelectedIndex = 0;

            // If there are app languages that the user speaks, show them next

            // Note: the first (non-override) language, if set as the primary language override
            // would give the same result as not having any primary language override. There's
            // still a difference, though: If the user changes their language preferences, the 
            // default setting (no override) would mean that the actual primary app language
            // could change. But if it's set as an override, then it will remain the primary
            // app language after the user changes their language preferences.

            foreach (var languageTag in ApplicationLanguages.Languages)
            {
                AddItemForLanguageTag(languageTag);
            }

            // Now add a divider followed by all the app manifest languages (in case the user
            // wants to pick a language not currently in their profile)

            // NOTE: If an app is deployed using a bundle with resource packages, the following
            // addition to the list may not be useful: The set of languages returned by 
            // ApplicationLanguages.ManifestLanguages will consist of only those manifest 
            // languages in the main app package or in the resource packages that are installed 
            // and registered for the current user. Language resource packages get deployed for 
            // the user if the language is in the user's profile. Therefore, the only difference 
            // from the set returned by ApplicationLanguages.Languages above would depend on 
            // which languages are included in the main app package.

            LanguageOverrideCombo.Items.Add(new ComboBoxItem { Content = "——————", Tag = "-" });

            // In a production app, this list should be sorted, but that's beyond the
            // focus of this sample.
            foreach (var languageTag in ApplicationLanguages.ManifestLanguages)
            {
                AddItemForLanguageTag(languageTag);
            }

            LanguageOverrideCombo.SelectionChanged += LanguageOverrideCombo_SelectionChanged;

            lastSelectedIndex = LanguageOverrideCombo.SelectedIndex;
        }

        private void UpdateCurrentAppLanguageMessage()
        {
            this.Scenario8AppLanguagesTextBlock.Text = "Current app language(s): " + GetAppLanguagesAsFormattedString();
        }

        private string GetAppLanguagesAsFormattedString()
        {
            return String.Join(", ", ApplicationLanguages.Languages);
        }
    }
}
