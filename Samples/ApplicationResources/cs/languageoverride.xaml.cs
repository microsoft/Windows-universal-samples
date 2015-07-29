using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.Globalization;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace SDKTemplate
{
    public sealed partial class LanguageOverride : UserControl
    {
        List<ComboBoxValue> comboBoxValues;
        int lastSelectionIndex;

        public delegate void LanguageOverrideChangedEventHandler(object sender, EventArgs e);
        public event LanguageOverrideChangedEventHandler LanguageOverrideChanged;

        public LanguageOverride()
        {
            this.InitializeComponent();
            Loaded += Control_Loaded;
        }

        void Control_Loaded(object sender, RoutedEventArgs e)
        {
            comboBoxValues = new List<ComboBoxValue>();

            // First show the default setting
            comboBoxValues.Add(new ComboBoxValue() { DisplayName = "Use language preferences (recommended)", LanguageTag = "" });


            // If there are app languages that the user speaks, show them next

            // Note: the first (non-override) language, if set as the primary language override
            // would give the same result as not having any primary language override. There's
            // still a difference, though: If the user changes their language preferences, the 
            // default setting (no override) would mean that the actual primary app language
            // could change. But if it's set as an override, then it will remain the primary
            // app language after the user changes their language preferences.
            
            for (var i = 0; i < ApplicationLanguages.Languages.Count; i++)
            {
                    this.LanguageOverrideComboBox_AddLanguage(new Windows.Globalization.Language(ApplicationLanguages.Languages[i]));
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
            
            comboBoxValues.Add(new ComboBoxValue() { DisplayName = "——————", LanguageTag = "-" });

            // Create a List and sort it before adding items
            List<Windows.Globalization.Language> manifestLanguageObjects = new List<Windows.Globalization.Language>();
            foreach (var lang in ApplicationLanguages.ManifestLanguages)
            {
                manifestLanguageObjects.Add(new Windows.Globalization.Language(lang));
            }
            IEnumerable<Windows.Globalization.Language> orderedManifestLanguageObjects = manifestLanguageObjects.OrderBy(lang => lang.DisplayName);
            foreach (var lang in orderedManifestLanguageObjects)
            {
                this.LanguageOverrideComboBox_AddLanguage(lang);
            }

            LanguageOverrideComboBox.ItemsSource = comboBoxValues;
            LanguageOverrideComboBox.SelectedIndex = comboBoxValues.FindIndex(FindCurrent);
            LanguageOverrideComboBox.SelectionChanged += LanguageOverrideComboBox_SelectionChanged;

            lastSelectionIndex = LanguageOverrideComboBox.SelectedIndex;
        }

        private void LanguageOverrideComboBox_AddLanguage(Windows.Globalization.Language lang)
        {
            comboBoxValues.Add(new ComboBoxValue() { DisplayName = lang.NativeName, LanguageTag = lang.LanguageTag });
        }

        private static bool FindCurrent(ComboBoxValue value)
        {

            if (value.LanguageTag == Windows.Globalization.ApplicationLanguages.PrimaryLanguageOverride)
            {
                return true;
            }
            return false;

        }

        private void LanguageOverrideComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBox combo = sender as ComboBox;

            // Don't accept the list item for the divider (tag = "-")
            if (combo.SelectedValue.ToString() == "-")
            {
                combo.SelectedIndex = lastSelectionIndex;
            }
            else
            {
                lastSelectionIndex = combo.SelectedIndex;

                // Set the persistent application language override
                Windows.Globalization.ApplicationLanguages.PrimaryLanguageOverride = combo.SelectedValue.ToString();

                if (LanguageOverrideChanged != null) LanguageOverrideChanged(this, new EventArgs());
            }
 
        }
    }

    public class ComboBoxValue
    {
        public string DisplayName { get; set; }
        public string LanguageTag { get; set; }
    }
}
