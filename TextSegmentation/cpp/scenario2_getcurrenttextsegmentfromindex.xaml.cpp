// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario2_GetCurrentTextSegmentFromIndex.xaml.h"
#include "MainPage.xaml.h"

using namespace Platform;

using namespace SDKTemplate;
using namespace TextSegmentation;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Scenario2_GetCurrentTextSegmentFromIndex::Scenario2_GetCurrentTextSegmentFromIndex()
{
    InitializeComponent();
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
void Scenario2_GetCurrentTextSegmentFromIndex::WordSegmentButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Initialize and obtain input values
    String^ notifyText = ref new String();

    // Obtain the input string value, check for non-emptiness
    String^ inputStringText = inputStringBox->Text;
    if (inputStringText->IsEmpty())
    {
        notifyText = "\nCannot compute word segments: input string is empty.";
        MainPage::Current->NotifyUser(notifyText, NotifyType::ErrorMessage);
        return;
    }

    // Obtain the language tag value, check for non-emptiness and that it is valid
    // Ex. Valid Values:
    //     "en-US" (English (United States))
    //     "fr-FR" (French (France))
    //     "de-DE" (German (Germany))
    //     "ja-JP" (Japanese (Japan))
    //     "ar-SA" (Arabic (Saudi Arabia))
    //     "zh-CN" (China (PRC))
    String^ languageTagText = languageTagBox->Text;
    if (languageTagText->IsEmpty())
    {
        notifyText += "\nLanguage tag input is empty ... using generic-language segmentation rules.";
        languageTagText = "und";    // This is used for non language-specific locales. 'und' is short for 'undetermined'.
    }
    else
    {
        if (!Windows::Globalization::Language::IsWellFormed(languageTagText))
        {
            notifyText = "\nLanguage tag is not well formed: \"" + languageTagText + "\"";
            MainPage::Current->NotifyUser(notifyText, NotifyType::ErrorMessage);
            return;
        }
    }

    // Obtain the input Index
    String^ inputIndexString = indexBox->Text;
    unsigned int inputIndex = 0;
    if (inputIndexString->IsEmpty())
    {
        notifyText += "\nNo input index provided ... using first segment reference (index = 0) as default.";
    }
    else
    {
        try
        {
            inputIndex = _wtoi(indexBox->Text->Data());
        }
        catch (Exception^)
        {
            notifyText = "\nInvalid index supplied.\n\nPlease check that this value is valid, and non-negative.";
            MainPage::Current->NotifyUser(notifyText, NotifyType::ErrorMessage);
            return;
        }

        if ((inputIndex < 0) || (inputIndex >= inputStringText->Length()))
        {
            notifyText = "\nInvalid index supplied ... cannot use a negative index, or an index that is out of bounds of the input string.\n\nPlease re-check the index value, and try again.";
            MainPage::Current->NotifyUser(notifyText, NotifyType::ErrorMessage);
            return;
        }
    }

    // Notify that we are going to calculate word segment
    notifyText += "\n\nFinding the word segment for the given index ...\n";
    notifyText += "\nInput: \"" + inputStringText + "\"";
    notifyText += "\nLanguage Tag: \"" + languageTagText + "\"";
    notifyText += "\nIndex: " + inputIndex + "\n";

    // Construct the WordsSegmenter instance
    auto segmenter = ref new Windows::Data::Text::WordsSegmenter(languageTagText);

    // Obtain the token segment
    auto tokenSegment = segmenter->GetTokenAt(inputStringText, inputIndex);
    notifyText += "\nIndexed segment: \"" + tokenSegment->Text + "\"";

    // Set output box text to the contents of the StringBuilder instance
    MainPage::Current->NotifyUser(notifyText, NotifyType::StatusMessage);
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
void Scenario2_GetCurrentTextSegmentFromIndex::SelectionSegmentButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Initialize and obtain input values
    String^ notifyText = ref new String();

    // Obtain the input string value, check for non-emptiness
    String^ inputStringText = inputStringBox->Text;
    if (inputStringText->IsEmpty())
    {
        notifyText = "\nCannot compute word segments: input string is empty.";
        MainPage::Current->NotifyUser(notifyText, NotifyType::ErrorMessage);
        return;
    }

    // Obtain the language tag value, check for non-emptiness and that it is valid
    // Ex. Valid Values:
    //     "en-US" (English (United States))
    //     "fr-FR" (French (France))
    //     "de-DE" (German (Germany))
    //     "ja-JP" (Japanese (Japan))
    //     "ar-SA" (Arabic (Saudi Arabia))
    //     "zh-CN" (China (PRC))
    String^ languageTagText = languageTagBox->Text;
    if (languageTagText->IsEmpty())
    {
        notifyText += "\nLanguage tag input is empty ... using generic-language segmentation rules.";
        languageTagText = "und";    // This is used for non language-specific locales. 'und' is short for 'undetermined'.
    }
    else
    {
        if (!Windows::Globalization::Language::IsWellFormed(languageTagText))
        {
            notifyText = "\nLanguage tag is not well formed: \"" + languageTagText + "\"";
            MainPage::Current->NotifyUser(notifyText, NotifyType::ErrorMessage);
            return;
        }
    }

    // Obtain the input Index
    String^ inputIndexString = indexBox->Text;
    unsigned int inputIndex = 0;
    if (inputIndexString->IsEmpty())
    {
        notifyText += "\nNo input index provided ... using first segment reference (index = 0) as default.";
    }
    else
    {
        try
        {
            inputIndex = _wtoi(indexBox->Text->Data());
        }
        catch (Exception^)
        {
            notifyText = "\nInvalid index supplied.\n\nPlease check that this value is valid, and non-negative.";
            MainPage::Current->NotifyUser(notifyText, NotifyType::ErrorMessage);
            return;
        }

        if ((inputIndex < 0) || (inputIndex >= inputStringText->Length()))
        {
            notifyText = "\nInvalid index supplied ... cannot use a negative index, or an index that is out of bounds of the input string.\n\nPlease re-check the index value, and try again.";
            MainPage::Current->NotifyUser(notifyText, NotifyType::ErrorMessage);
            return;
        }
    }

    // Notify that we are going to calculate selection segment
    notifyText += "\n\nFinding the selection segment for the given index ...\n";
    notifyText += "\nInput: \"" + inputStringText + "\"";
    notifyText += "\nLanguage Tag: \"" + languageTagText + "\"";
    notifyText += "\nIndex: " + inputIndex + "\n";

    // Construct the SelectableWordsSegmenter instance
    auto segmenter = ref new Windows::Data::Text::SelectableWordsSegmenter(languageTagText);

    // Obtain the token segment
    auto tokenSegment = segmenter->GetTokenAt(inputStringText, inputIndex);
    notifyText += "\nIndexed segment: \"" + tokenSegment->Text + "\"";

    // Set output box text to the contents of the StringBuilder instance
    MainPage::Current->NotifyUser(notifyText, NotifyType::StatusMessage);
}
