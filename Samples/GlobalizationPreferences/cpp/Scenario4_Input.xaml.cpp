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
#include "Scenario4_Input.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;

Scenario4_Input::Scenario4_Input()
{
    InitializeComponent();
}

void Scenario4_Input::ShowResults()
{
    // This scenario uses the Windows.Globalization.Language class to obtain the user's current 
    // input language.  The language tag returned reflects the current input language specified 
    // by the user.
    String^ userInputLanguage = Windows::Globalization::Language::CurrentInputMethodLanguageTag;

    // Display the results
    OutputTextBlock->Text = "User's current input language: " + userInputLanguage;
}