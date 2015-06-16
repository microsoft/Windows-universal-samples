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
    public sealed partial class Scenario2_GetCurrentTextSegmentFromIndex
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario2_GetCurrentTextSegmentFromIndex()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// This is the click handler for the 'Word Segments' button.
        ///
        /// When this button is activated, the Text Segmentation API will calculate
        /// the word segment from the given input string and character index for that string, 
        /// and return the WordSegment object that contains the index within its text bounds.
        /// Segment breaking behavior is based off of the language-tag input, which defines 
        /// which language rules to use.
        ///
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">Event data that describes the click action on the button.</param>
        private void WordSegmentButton_Click(object sender, RoutedEventArgs e)
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

            // Obtain the input Index
            String inputIndexString = indexBox.Text;
            uint inputIndex = 0;
            if (String.IsNullOrEmpty(inputIndexString))
            {
                notifyText.AppendLine("No input index provided ... using first segment reference (index = 0) as default.");
            }
            else
            {
                try
                {
                    inputIndex = Convert.ToUInt32(indexBox.Text);
                }
                catch (FormatException)
                {
                    notifyText = new StringBuilder("Invalid index supplied.\n\nPlease check that this value is valid, and non-negative.");
                    rootPage.NotifyUser(notifyText.ToString(), NotifyType.ErrorMessage);
                    return;
                }
                catch (OverflowException)
                {
                    notifyText = new StringBuilder("Invalid index supplied: Negative-valued index.\n\nPlease check that this value is valid, and non-negative.");
                    rootPage.NotifyUser(notifyText.ToString(), NotifyType.ErrorMessage);
                    return;
                }

                if ((inputIndex < 0) || (inputIndex >= inputStringText.Length))
                {
                    notifyText = new StringBuilder("Invalid index supplied ... cannot use a negative index, or an index that is out of bounds of the input string.\n\nPlease re-check the index value, and try again.");
                    rootPage.NotifyUser(notifyText.ToString(), NotifyType.ErrorMessage);
                    return;
                }
            }

            // Notify that we are going to calculate word segment
            notifyText.AppendLine("\nFinding the word segment for the given index ...\n");
            notifyText.AppendLine("Input: \"" + inputStringText + "\"");
            notifyText.AppendLine("Language Tag: \"" + languageTagText + "\"");
            notifyText.AppendLine("Index: " + inputIndex + "\n");

            // Construct the WordsSegmenter instance
            var segmenter = new Windows.Data.Text.WordsSegmenter(languageTagText);

            // Obtain the token segment
            var tokenSegment = segmenter.GetTokenAt(inputStringText, inputIndex);
            notifyText.AppendLine("Indexed segment: \"" + tokenSegment.Text + "\"");

            // Set output box text to the contents of the StringBuilder instance
            rootPage.NotifyUser(notifyText.ToString(), NotifyType.StatusMessage);
        }

        /// <summary>
        /// This is the click handler for the 'Selection Segment' button.
        ///
        /// When this button is activated, the Text Segmentation API will calculate
        /// the selection segment from the given input string and character index for that string, 
        /// and return the SelectableWordSegment object that contains the index within its text bounds.
        /// Segment breaking behavior is based off of the language-tag input, which defines 
        /// which language rules to use.
        ///
        /// Selection segments differ from word segments in that they describe the bounds
        /// between active-selection snapping behavior.
        ///
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">Event data that describes the click action on the button.</param>
        private void SelectionSegmentButton_Click(object sender, RoutedEventArgs e)
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

            // Obtain the input Index
            String inputIndexString = indexBox.Text;
            uint inputIndex = 0;
            if (String.IsNullOrEmpty(inputIndexString))
            {
                notifyText.AppendLine("No input index provided ... using first segment reference (index = 0) as default.");
            }
            else
            {
                try
                {
                    inputIndex = Convert.ToUInt32(indexBox.Text);
                }
                catch (FormatException)
                {
                    notifyText = new StringBuilder("Invalid index supplied.\n\nPlease check that this value is valid, and non-negative.");
                    rootPage.NotifyUser(notifyText.ToString(), NotifyType.ErrorMessage);
                    return;
                }
                catch (OverflowException)
                {
                    notifyText = new StringBuilder("Invalid index supplied: Negative-valued index.\n\nPlease check that this value is valid, and non-negative.");
                    rootPage.NotifyUser(notifyText.ToString(), NotifyType.ErrorMessage);
                    return;
                }

                if ((inputIndex < 0) || (inputIndex >= inputStringText.Length))
                {
                    notifyText = new StringBuilder("Invalid index supplied ... cannot use a negative index, or an index that is out of bounds of the input string.\n\nPlease re-check the index value, and try again.");
                    rootPage.NotifyUser(notifyText.ToString(), NotifyType.ErrorMessage);
                    return;
                }
            }

            // Notify that we are going to calculate selection segment
            notifyText.AppendLine("\nFinding the selection segment for the given index ...\n");
            notifyText.AppendLine("Input: \"" + inputStringText + "\"");
            notifyText.AppendLine("Language Tag: \"" + languageTagText + "\"");
            notifyText.AppendLine("Index: " + inputIndex + "\n");

            // Construct the SelectableWordsSegmenter instance
            var segmenter = new Windows.Data.Text.SelectableWordsSegmenter(languageTagText);

            // Obtain the token segment
            var tokenSegment = segmenter.GetTokenAt(inputStringText, inputIndex);
            notifyText.AppendLine("Indexed segment: \"" + tokenSegment.Text + "\"");

            // Set output box text to the contents of the StringBuilder instance
            rootPage.NotifyUser(notifyText.ToString(), NotifyType.StatusMessage);
        }
    }
}
