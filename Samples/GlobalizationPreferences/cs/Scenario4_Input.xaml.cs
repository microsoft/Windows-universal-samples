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

using Windows.Foundation;
using Windows.Globalization;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario4_Input : Page
    {
        public Scenario4_Input()
        {
            this.InitializeComponent();
        }

        private void ShowResults()
        {
            // This scenario uses the Windows.Globalization.Language class to obtain the user's current 
            // input language.  The language tag returned reflects the current input language specified 
            // by the user.
            var userInputLanguage = Windows.Globalization.Language.CurrentInputMethodLanguageTag;

            // Display the results
            OutputTextBlock.Text = "User's current input language: " + userInputLanguage;
        }
    }
}
