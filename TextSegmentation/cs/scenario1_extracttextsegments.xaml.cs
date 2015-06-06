//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using System.Text;

namespace TextSegmentation
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1_ExtractTextSegments
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario1_ExtractTextSegments()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// This is the click handler for the 'Word Segments' button.
        ///
        /// When this button is activated, the Text Segmentation API will calculate
        /// the word segments from the given input string and language tag, and then
        /// print out the word and alternate-form segments for that language.
        ///
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">Event data that describes the click action on the button.</param>
        private void WordSegmentsButton_Click(object sender, RoutedEventArgs e)
        {
            // Initialize and obtain input values
            StringBuilder notifyText = new StringBuilder();

            // Obtain the input string value, check for non-emptiness
            String inputStringText = inputStringBox.Text;
            if (String.IsNullOrEmpty(inputStringText))
            {
                notifyText = new StringBuilder("Cannot compute word segments: input string is empty.");
                rootPage.NotifyUser(notifyText.ToString(), NotifyType.ErrorMessage);
                return;
            }

            // Obtain the language tag value, check for non-emptiness
            // Ex. Valid Values:
            //     "en-US" (English (United States))
            //     "fr-FR" (French (France))
            //     "de-DE" (German (Germany))
            //     "ja-JP" (Japanese (Japan))
            //     "ar-SA" (Arabic (Saudi Arabia))
            //     "zh-CN" (China (PRC))
            String languageTagText = languageTagBox.Text;
            if (String.IsNullOrEmpty(languageTagText))
            {
                notifyText.AppendLine("Language tag input is empty ... using generic-language segmentation rules.");
                languageTagText = "und";    // This is used for non language-specific locales. 'und' is short for 'undetermined'.
            }
            else
            {
                if (!Windows.Globalization.Language.IsWellFormed(languageTagText))
                {
                    notifyText = new StringBuilder("Language tag is not well formed: \"" + languageTagText + "\"");
                    rootPage.NotifyUser(notifyText.ToString(), NotifyType.ErrorMessage);
                    return;
                }
            }

            // Notify that we are going to calculate word segments
            notifyText.AppendLine("\nCalculating word segments ...\n");
            notifyText.AppendLine("Input: \"" + inputStringText + "\"");
            notifyText.AppendLine("Language Tag: \"" + languageTagText + "\"");

            // Construct the WordsSegmenter instance
            var segmenter = new Windows.Data.Text.WordsSegmenter(languageTagText);
            var wordSegments = segmenter.GetTokens(inputStringText);

            // Iterate over the tokenized segments
            foreach (var wordSegment in wordSegments)
            {
                notifyText.AppendLine("\tWord---->\"" + wordSegment.Text + "\"");
                foreach (var alternateForm in wordSegment.AlternateForms)
                {
                    notifyText.AppendLine("\t\tAlternate---->\"" + alternateForm.AlternateText + "\"");
                }
            }

            // send notifyText to the output pane
            rootPage.NotifyUser(notifyText.ToString(), NotifyType.StatusMessage);
        }

        /// <summary>
        /// This is the click handler for the 'Selection Segments' button.
        ///
        /// When this button is activated, the Text Segmentation API will calculate
        /// the selection segments from the given input string and language tag, and then
        /// print out the word and alternate-form segments for that language.
        ///
        /// Selection segments differ from word segments in that they describe the bounds
        /// between active-selection snapping behavior.
        ///
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">Event data that describes the click action on the button.</param>
        private void SelectionSegmentsButton_Click(object sender, RoutedEventArgs e)
        {
            // Initialize and obtain input values
            StringBuilder notifyText = new StringBuilder();

            // Obtain the input string value, check for non-emptiness
            String inputStringText = inputStringBox.Text;
            if (String.IsNullOrEmpty(inputStringText))
            {
                notifyText = new StringBuilder("Cannot compute selection segments: input string is empty.");
                rootPage.NotifyUser(notifyText.ToString(), NotifyType.ErrorMessage);
                return;
            }

            // Obtain the language tag value, check for non-emptiness
            // Ex. Valid Values:
            //     "en-US" (English (United States))
            //     "fr-FR" (French (France))
            //     "de-DE" (German (Germany))
            //     "ja-JP" (Japanese (Japan))
            //     "ar-SA" (Arabic (Saudi Arabia))
            //     "zh-CN" (China (PRC))
            String languageTagText = languageTagBox.Text;
            if (String.IsNullOrEmpty(languageTagText))
            {
                notifyText.AppendLine("Language tag input is empty ... using generic-language segmentation rules.");
                languageTagText = "und";    // This is used for non language-specific locales. 'und' is short for 'undetermined'.
            }
            else
            {
                if (!Windows.Globalization.Language.IsWellFormed(languageTagText))
                {
                    notifyText = new StringBuilder("Language tag is not well formed: \"" + languageTagText + "\"");
                    rootPage.NotifyUser(notifyText.ToString(), NotifyType.ErrorMessage);
                    return;
                }
            }

            // Notify that we are going to calculate selection segments
            notifyText.AppendLine("\nCalculating selection segments ...\n");
            notifyText.AppendLine("Input: \"" + inputStringText + "\"");
            notifyText.AppendLine("Language Tag: \"" + languageTagText + "\"");

            // Construct the SelectableWordsSegmenter instance
            var segmenter = new Windows.Data.Text.SelectableWordsSegmenter(languageTagText);
            var selectionSegments = segmenter.GetTokens(inputStringText);

            // Iterate over the tokenized segments
            foreach (var selectionSegment in selectionSegments)
            {
                notifyText.AppendLine("\tWord---->\"" + selectionSegment.Text + "\"");
            }

            // Set output box text to the contents of the StringBuilder instance
            rootPage.NotifyUser(notifyText.ToString(), NotifyType.StatusMessage);
        }
    }
}
