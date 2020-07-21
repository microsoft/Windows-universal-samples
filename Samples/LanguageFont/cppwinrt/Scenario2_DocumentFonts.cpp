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
#include "Scenario2_DocumentFonts.h"
#include "Scenario2_DocumentFonts.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization::Fonts;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_DocumentFonts::Scenario2_DocumentFonts()
    {
        InitializeComponent();
    }

    void Scenario2_DocumentFonts::ApplyFonts_Click(IInspectable const&, RoutedEventArgs const&)
    {

        LanguageFontGroup languageFontGroup(L"hi");

        ApplyLanguageFont(HeadingTextBlock(), languageFontGroup.DocumentHeadingFont());

        // Not all scripts have recommended fonts for "document alternate"
        // categories, so need to verify before using it. Note that Hindi does
        // have document alternate fonts, so in this case the fallback logic is
        // unnecessary, but (for example) Japanese does not have recommendations
        // for the document alternate 2 category.
        LanguageFont documentFont = languageFontGroup.DocumentAlternate2Font();
        if (documentFont == nullptr)
        {
            documentFont = languageFontGroup.DocumentAlternate1Font();
        }
        if (documentFont == nullptr)
        {
            documentFont = languageFontGroup.ModernDocumentFont();
        }
        ApplyLanguageFont(DocumentTextBlock(), documentFont);
    }
}
