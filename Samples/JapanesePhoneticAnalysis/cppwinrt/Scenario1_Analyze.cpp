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
#include "Scenario1_Analyze.h"
#include "Scenario1_Analyze.g.cpp"
#include "MainPage.h"
#include <sstream>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_Analyze::Scenario1_Analyze()
    {
        InitializeComponent();
    }

    void Scenario1_Analyze::AnalyzeButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        hstring input = InputTextBox().Text();
        std::wostringstream output;

        // monoRuby = false means that each element in the result corresponds to a single Japanese word.
        // monoRuby = true means that each element in the result corresponds to one or more characters which are pronounced as a single unit.
        bool monoRuby = MonoRubyRadioButton().IsChecked().Value();

        // Analyze the Japanese text according to the specified algorithm.
        // The maximum length of the input string is 100 characters. 
        IVectorView<JapanesePhoneme> words = JapanesePhoneticAnalyzer::GetWords(input, monoRuby);
        for (JapanesePhoneme&& word : words)
        {
            // Put each phrase on its own line.
            if (output.tellp() != 0 && word.IsPhraseStart())
            {
                output << std::endl;
            }
            // DisplayText is the display text of the word, which has same characters as the input of GetWords().
            // YomiText is the reading text of the word, as known as Yomi, which typically consists of Hiragana characters.
            // However, please note that the reading can contains some non-Hiragana characters for some display texts such as emoticons or symbols.
            output << std::wstring_view{ word.DisplayText() } << L"(" << std::wstring_view{ word.YomiText() } << ")";
        }

        // Display the result.
        OutputTextBlock().Text(output.str());
        if (!input.empty() && words.Size() == 0)
        {
            // If the result from GetWords() is empty but the input is not empty,
            // it means the given input is too long to analyze.
            MainPage::Current().NotifyUser(L"Failed to get words from the input text.  The input text is too long to analyze.", NotifyType::ErrorMessage);
        }
        else
        {
            // Otherwise, the analysis has been done successfully.
            MainPage::Current().NotifyUser(L"Got words from the input text successfully.", NotifyType::StatusMessage);
        }
    }
}
