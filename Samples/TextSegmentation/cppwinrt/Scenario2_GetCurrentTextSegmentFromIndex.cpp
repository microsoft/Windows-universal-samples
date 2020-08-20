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

#include "pch.h"
#include "Scenario2_GetCurrentTextSegmentFromIndex.h"
#include "Scenario2_GetCurrentTextSegmentFromIndex.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_GetCurrentTextSegmentFromIndex::Scenario2_GetCurrentTextSegmentFromIndex()
    {
        InitializeComponent();
    }

    // This is the click handler for the 'Word Segment' button.
    //
    // When this button is activated, the Text Segmentation API will calculate
    // the word segment from the given input string and character index for that string, 
    // and return the WordSegment object that contains the index within its text bounds.
    // Segment breaking behavior is based off of the language-tag input, which defines 
    // which language rules to use.
    void Scenario2_GetCurrentTextSegmentFromIndex::WordSegmentButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        std::wostringstream result;

        // Obtain the input string value, check for non-emptiness
        hstring inputStringText = inputStringBox().Text();
        if (inputStringText.empty())
        {
            rootPage.NotifyUser(L"Cannot compute word segments: input string is empty.", NotifyType::ErrorMessage);
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
        hstring languageTagText = languageTagBox().Text();
        if (languageTagText.empty())
        {
            result << L"Language tag input is empty ... using generic-language segmentation rules.\n";
            languageTagText = L"und";   // This is used for non language-specific locales. 'und' is short for 'undetermined'.
        }
        else
        {
            if (!Windows::Globalization::Language::IsWellFormed(languageTagText))
            {
                rootPage.NotifyUser(L"Language tag is not well formed: \"" + languageTagText + L"\"", NotifyType::ErrorMessage);
                return;
            }
        }

        // Obtain the input Index
        hstring inputIndexString = indexBox().Text();
        uint32_t inputIndex = static_cast<uint32_t>(_wtoi(inputIndexString.c_str()));
        if (errno == EINVAL)
        {
            result << L"No input index provided ... using first segment reference (index = 0) as default.\n";
        }
        else if (inputIndex > inputStringText.size())
        {
            rootPage.NotifyUser(L"Invalid index supplied ... cannot use a negative index, or an index that is out of bounds of the input string.\n\nPlease re-check the index value, and try again.",
                NotifyType::ErrorMessage);
            return;
        }

        // Notify that we are going to calculate word segment
        result
            << L"\n\nFinding the word segment for the given index ...\n"
            << L"Input: \"" << std::wstring_view(inputStringText) << L"\"\n"
            << L"Language Tag: \"" << std::wstring_view(languageTagText) << L"\"\n"
            << L"Index: " << inputIndex << L"\n";

        // Construct the WordsSegmenter instance
        Windows::Data::Text::WordsSegmenter segmenter(languageTagText);

        // Obtain the token segment
        auto tokenSegment = segmenter.GetTokenAt(inputStringText, inputIndex);
        result << L"\nIndexed segment: \"" << std::wstring_view(tokenSegment.Text()) << L"\"";

        // Set results to output box
        rootPage.NotifyUser(result.str(), NotifyType::StatusMessage);
    }

    // This is the click handler for the 'Selection Segment' button.
    //
    // When this button is activated, the Text Segmentation API will calculate
    // the selection segment from the given input string and character index for that string, 
    // and return the SelectableWordSegment object that contains the index within its text bounds.
    // Segment breaking behavior is based off of the language-tag input, which defines 
    // which language rules to use.
    //
    // Selection segments differ from word segments in that they describe the bounds
    // between active-selection snapping behavior.
    void Scenario2_GetCurrentTextSegmentFromIndex::SelectionSegmentButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        std::wostringstream result;

        // Obtain the input string value, check for non-emptiness
        hstring inputStringText = inputStringBox().Text();
        if (inputStringText.empty())
        {
            rootPage.NotifyUser(L"Cannot compute word segments: input string is empty.", NotifyType::ErrorMessage);
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
        hstring languageTagText = languageTagBox().Text();
        if (languageTagText.empty())
        {
            result << L"Language tag input is empty ... using generic-language segmentation rules.\n";
            languageTagText = L"und";   // This is used for non language-specific locales. 'und' is short for 'undetermined'.
        }
        else
        {
            if (!Windows::Globalization::Language::IsWellFormed(languageTagText))
            {
                rootPage.NotifyUser(L"Language tag is not well formed: \"" + languageTagText + L"\"", NotifyType::ErrorMessage);
                return;
            }
        }

        // Obtain the input Index
        hstring inputIndexString = indexBox().Text();
        uint32_t inputIndex = static_cast<uint32_t>(_wtoi(inputIndexString.c_str()));
        if (errno == EINVAL)
        {
            result << L"No input index provided ... using first segment reference (index = 0) as default.\n";
        }
        else if (inputIndex > inputStringText.size())
        {
            rootPage.NotifyUser(L"Invalid index supplied ... cannot use a negative index, or an index that is out of bounds of the input string.\n\nPlease re-check the index value, and try again.",
                NotifyType::ErrorMessage);
            return;
        }

        // Notify that we are going to calculate word segment
        result
            << L"\n\nFinding the selection segment for the given index ...\n"
            << L"Input: \"" << std::wstring_view(inputStringText) << L"\"\n"
            << L"Language Tag: \"" << std::wstring_view(languageTagText) << L"\"\n"
            << L"Index: " << inputIndex << L"\n";

        // Construct the SelectableWordsSegmenter instance
        Windows::Data::Text::SelectableWordsSegmenter segmenter(languageTagText);

        // Obtain the token segment
        auto tokenSegment = segmenter.GetTokenAt(inputStringText, inputIndex);
        result << L"\nIndexed segment: \"" << std::wstring_view(tokenSegment.Text()) << L"\"";

        // Set results to output box
        rootPage.NotifyUser(result.str(), NotifyType::StatusMessage);
    }
}
