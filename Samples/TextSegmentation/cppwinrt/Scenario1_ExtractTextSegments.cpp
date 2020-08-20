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
#include "Scenario1_ExtractTextSegments.h"
#include "Scenario1_ExtractTextSegments.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_ExtractTextSegments::Scenario1_ExtractTextSegments()
    {
        InitializeComponent();
    }

    // This is the click handler for the 'Word Segments' button.
    //
    // When this button is activated, the Text Segmentation API will calculate
    // the word segments from the given input string and language tag, and then
    // print out the word and alternate-form segments for that language.
    void Scenario1_ExtractTextSegments::WordSegmentsButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        std::wostringstream result;

        // Obtain the input string value, check for non-emptiness
        hstring inputStringText = inputStringBox().Text();
        if (inputStringText.empty())
        {
            rootPage.NotifyUser(L"Cannot compute word selection segments: input string is empty.", NotifyType::ErrorMessage);
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

        // Notify that we are going to calculate word segments
        result
            << L"\nCalculating word segments ...\n\n"
            << L"Input: \"" << std::wstring_view(inputStringText) << L"\"\n"
            << L"Language Tag: \"" << std::wstring_view(languageTagText) << L"\"\n";

        // Construct the WordsSegmenter instance
        Windows::Data::Text::WordsSegmenter segmenter(languageTagText);
        auto wordSegments = segmenter.GetTokens(inputStringText);

        // Iterate over the tokenized segments
        for (auto&& wordSegment : wordSegments)
        {
            result << L"\tWord---->\"" << std::wstring_view(wordSegment.Text()) << L"\"\n";
            for  (auto&& alternateForm : wordSegment.AlternateForms())
            {
                result << L"\t\tAlternate---->\"" << std::wstring_view(alternateForm.AlternateText()) << L"\"\n";
            }
        }

        // Set results to output box
        rootPage.NotifyUser(result.str(), NotifyType::StatusMessage);
    }

    // This is the click handler for the 'Selection Segments' button.
    //
    // When this button is activated, the Text Segmentation API will calculate
    // the selection segments from the given input string and language tag, and then
    // print out the word and alternate-form segments for that language.
    //
    // Selection segments differ from word segments in that they describe the bounds
    // between active-selection snapping behavior.
    void Scenario1_ExtractTextSegments::SelectionSegmentsButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        std::wostringstream result;

        // Obtain the input string value, check for non-emptiness
        hstring inputStringText = inputStringBox().Text();
        if (inputStringText.empty())
        {
            rootPage.NotifyUser(L"Cannot compute selection segments: input string is empty.", NotifyType::ErrorMessage);
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

        // Notify that we are going to calculate selection segments
        result
            << L"\nCalculating selection segments ...\n\n"
            << L"Input: \"" << std::wstring_view(inputStringText) << L"\"\n"
            << L"Language Tag: \"" << std::wstring_view(languageTagText) << L"\"\n";

        // Construct the SelectableWordsSegmenter instance
        Windows::Data::Text::SelectableWordsSegmenter segmenter(languageTagText);
        auto selectionSegments = segmenter.GetTokens(inputStringText);

        // Iterate over the tokenized segments
        for (auto&& selectionSegment : selectionSegments)
        {
            result << L"\tWord---->\"" << std::wstring_view(selectionSegment.Text()) << L"\"\n";
        }

        // Set results to output box
        rootPage.NotifyUser(result.str(), NotifyType::StatusMessage);
    }
}
