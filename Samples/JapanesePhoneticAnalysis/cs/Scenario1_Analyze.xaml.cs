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
using System.Collections.Generic;
using System.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.Globalization;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Analyze : Page
    {
        public Scenario1_Analyze()
        {
            this.InitializeComponent();
        }

        private void AnalyzeButton_Click(object sender, RoutedEventArgs e)
        {
            string input = this.InputTextBox.Text;
            StringBuilder output = new StringBuilder();

            // monoRuby = false means that each element in the result corresponds to a single Japanese word.
            // monoRuby = true means that each element in the result corresponds to one or more characters which are pronounced as a single unit.
            bool monoRuby = MonoRubyRadioButton.IsChecked == true;

            // Analyze the Japanese text according to the specified algorithm.
            IReadOnlyList<JapanesePhoneme> words = JapanesePhoneticAnalyzer.GetWords(input, monoRuby);
            foreach (JapanesePhoneme word in words)
            {
                // Put each phrase on its own line.
                if (output.Length != 0 && word.IsPhraseStart)
                {
                    output.AppendLine();
                }
                // DisplayText is the display text of the word, which has same characters as the input of GetWords().
                // YomiText is the reading text of the word, as known as Yomi, which typically consists of Hiragana characters.
                // However, please note that the reading can contains some non-Hiragana characters for some display texts such as emoticons or symbols.
                output.AppendFormat("{0}({1})", word.DisplayText, word.YomiText);
            }

            // Display the result.
            string outputString = output.ToString();
            this.OutputTextBlock.Text = outputString;
            if (input != "" && words.Count == 0)
            {
                // If the result from GetWords() is empty but the input is not empty,
                // it means the given input is too long to analyze.
                MainPage.Current.NotifyUser("Failed to get words from the input text.  The input text is too long to analyze.", NotifyType.ErrorMessage);
            }
            else
            {
                // Otherwise, the analysis has been done successfully.
                MainPage.Current.NotifyUser("Got words from the input text successfully.", NotifyType.StatusMessage);
            }
        }
    }
}
