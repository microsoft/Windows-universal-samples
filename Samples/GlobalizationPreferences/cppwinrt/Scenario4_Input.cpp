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
#include "Scenario4_Input.h"
#include "Scenario4_Input.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario4_Input::Scenario4_Input()
    {
        InitializeComponent();
    }

    void Scenario4_Input::ShowResults_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // This scenario uses the Windows.Globalization.Language class to obtain the user's current 
        // input language.  The language tag returned reflects the current input language specified 
        // by the user.
        hstring userInputLanguage = Language::CurrentInputMethodLanguageTag();

        // Display the results
        OutputTextBlock().Text(L"User's current input language: " + userInputLanguage);
    }
}
