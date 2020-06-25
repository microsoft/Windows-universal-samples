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
#include "Scenario1_UIFonts.h"
#include "Scenario1_UIFonts.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization::Fonts;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_UIFonts::Scenario1_UIFonts()
    {
        InitializeComponent();
    }

    void Scenario1_UIFonts::ApplyFonts_Click(IInspectable const&, RoutedEventArgs const&)
    {
        LanguageFontGroup languageFontGroup(L"ja-JP");

        // Change the Font value with selected font from LanguageFontGroup API
        ApplyLanguageFont(HeadingTextBlock(), languageFontGroup.UIHeadingFont());
        ApplyLanguageFont(BodyTextBlock(), languageFontGroup.UITextFont());

    }
}
