// Copyright (c) Microsoft Corporation. All rights reserved

using System;
using Windows.Data.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;

namespace TextSuggestion
{
    public sealed partial class Scenario1_Conversion : Page
    {
        private MainPage rootPage;
        private TextConversionGenerator generator;

        /// <summary>
        /// Page initializer.
        /// </summary>
        public Scenario1_Conversion()
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
        /// the conversion result for given input string and language tag, then 
        /// print them in result column.
        /// Using 'async' keyword to indicate that this method runs asynchronously
        /// and contains one 'await' expression.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">Event data that describes the click action on the button.</param>
        private async void Execute_Click(object sender, RoutedEventArgs e)
        {
            // Clean up result column.
            resultView.ItemsSource = null;

            // Clean notification area.
            rootPage.NotifyUser(string.Empty, NotifyType.StatusMessage);

            var input = Input.Text;

            ComboBoxItem selectedItem = methods.SelectedItem as ComboBoxItem;

            if ((generator != null) && (selectedItem != null) && !String.IsNullOrEmpty(input))
            {
                // Specify the candidate number to get.
                if ((selectedItem.Tag != null) && selectedItem.Tag.ToString().Equals("needMaxCount"))
                {
                    uint MaxCandidates = 0;
                    UInt32.TryParse(maxCandidates.Text, out MaxCandidates);

                    if (MaxCandidates != 0)
                    {
                        // Call the API with max candidate number we expect, and list the result when there are any candidates.
                        // Using 'await' expression here to suspend the execution of this method until the awaited task completes.
                        resultView.ItemsSource = await generator.GetCandidatesAsync(input, MaxCandidates);

                        if (resultView.Items.Count == 0)
                        {
                            rootPage.NotifyUser(string.Format("No candidates."), NotifyType.StatusMessage);
                        }
                    }
                    else
                    {
                        rootPage.NotifyUser(string.Format("Not a valid candidate number. Expecting positive integer."), NotifyType.ErrorMessage);
                    }
                }
                else
                {
                    // Call the API with default candidate number, and list the result when there are any candidates. 
                    resultView.ItemsSource = await generator.GetCandidatesAsync(input);

                    if (resultView.Items.Count == 0)
                    {
                        rootPage.NotifyUser(string.Format("No candidates."), NotifyType.StatusMessage);
                    }
                }
            }
        }

        /// <summary>
        /// Selecting the API interface to use.
        /// User can either specify the max candidate number to get, or use the default value.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">Event data that describes the click action on the button.</param>
        private void Methods_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBoxItem selectedItem = methods.SelectedItem as ComboBoxItem;

            if (maxCandidatesArea != null)
            {
                // Only show the candidate number input control when user chooses to specify the number.
                if ((selectedItem.Tag != null) && selectedItem.Tag.ToString().Equals("needMaxCount"))
                {
                    maxCandidatesArea.Visibility = Visibility.Visible;
                }
                else
                {
                    maxCandidatesArea.Visibility = Visibility.Collapsed;
                }
            }
        }

        /// <summary>
        /// This is the click handler for the 'Create Generator' button.
        /// When this button is activated, the Text Suggestion API will try
        /// to resolve the language tag provided by the user, and create a 
        /// Conversion Generator.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">Event data that describes the click action on the button.</param>
        private void CreateGeneratorButton_Click(object sender, RoutedEventArgs e)
        {
            // Try to parse the language and create generator accordingly.
            generator = new TextConversionGenerator(langTag.Text);

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
