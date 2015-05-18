// Copyright (c) Microsoft Corporation. All rights reserved

using System;
using Windows.Data.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario3_ReverseConversion : Page
    {
        private MainPage rootPage;
        private TextReverseConversionGenerator generator;

        /// <summary>
        /// Page initializer.
        /// </summary>
        public Scenario3_ReverseConversion()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Clean up the notification area when user navigate to prediction page
        /// </summary>
        /// <param name="e">Event data that describes the click action on the button.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            // Clean notification area.
            rootPage.NotifyUser(string.Empty, NotifyType.StatusMessage);
        }

        /// <summary>
        /// This is the click handler for the 'Execute' button.
        /// When this button is activated, the text suggestion generator will get
        /// the reverse conversion result for given input string and language tag, then 
        /// print them in result column.
        /// Using 'async' keyword to indicate that this method runs asynchronously
        /// and contains one 'await' expression.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">Event data that describes the click action on the button.</param>
        private async void Execute_Click(object sender, RoutedEventArgs e)
        {
            // Clean up result column.
            result.Text = string.Empty;

            // Clean notification area.
            rootPage.NotifyUser(string.Empty, NotifyType.StatusMessage);

            var input = Input.Text;

            ComboBoxItem selectedItem = methods.SelectedItem as ComboBoxItem;

            if ((generator != null) && (selectedItem != null) && (!String.IsNullOrEmpty(input)))
            {
                // Get the reverse conversion result through calling the API and print it in result area.
                // Using 'await' expression here to suspend the execution of this method until the awaited task completes.
                result.Text = await generator.ConvertBackAsync(input);
                if (result.Text.Length == 0)
                {
                    rootPage.NotifyUser(string.Format("No candidates"), NotifyType.StatusMessage);
                }
            }
        }

        /// <summary>       
        /// This is the click handler for the 'Create Generator' button.
        /// When this button is activated, the Text Suggestion API will try
        /// to resolve the language tag provided by the user, and create a 
        /// Reverse Conversion Generator.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">Event data that describes the click action on the button.</param>
        private void CreateGeneratorButton_Click(object sender, RoutedEventArgs e)
        {
            // Try to parse the language and create generator accordingly.
            generator = new TextReverseConversionGenerator(langTag.Text);

            // Only allow generator operation when the language is available and installed.
            if (generator == null)
            {
                if (GeneratorOperationArea != null)
                {
                    GeneratorOperationArea.Visibility = Visibility.Collapsed;
                }

                rootPage.NotifyUser(string.Format("Failed to instantiate a generator."), NotifyType.ErrorMessage);
            }
            else if (generator.ResolvedLanguage == "und")
            {
                if (GeneratorOperationArea != null)
                {
                    GeneratorOperationArea.Visibility = Visibility.Collapsed;
                }

                rootPage.NotifyUser(string.Format("Unsupported language."), NotifyType.ErrorMessage);
            }
            else if (generator.LanguageAvailableButNotInstalled)
            {
                if (GeneratorOperationArea != null)
                {
                    GeneratorOperationArea.Visibility = Visibility.Collapsed;
                }

                rootPage.NotifyUser(string.Format("Language available but not installed."), NotifyType.ErrorMessage);
            }
            else
            {
                if (GeneratorOperationArea != null)
                {
                    GeneratorOperationArea.Visibility = Visibility.Visible;
                }

                rootPage.NotifyUser(string.Format("Successfully instantiated a generator."), NotifyType.StatusMessage);
            }
        }
    }
}
