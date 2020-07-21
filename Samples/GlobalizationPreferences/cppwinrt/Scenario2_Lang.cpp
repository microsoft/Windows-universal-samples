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
#include "Scenario2_Lang.h"
#include "Scenario2_Lang.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::System::UserProfile;
using namespace Windows::UI::Xaml;

namespace
{
    hstring ReportLanguageData(Language const& lang)
    {
        return L"Display Name: " + lang.DisplayName() + L"\n" +
            L"Language Tag: " + lang.LanguageTag() + L"\n" +
            L"Native Name: " + lang.NativeName() + L"\n" +
            L"Script Code: " + lang.Script() + L"\n\n";
    }
}

namespace winrt::SDKTemplate::implementation
{
    Scenario2_Lang::Scenario2_Lang()
    {
        InitializeComponent();
    }

    void Scenario2_Lang::ShowResults_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // This scenario uses the Windows.System.UserProfile.GlobalizationPreferences class to
        // obtain the user's preferred language characteristics.
        hstring topUserLanguage = GlobalizationPreferences::Languages().GetAt(0);
        Windows::Globalization::Language userLanguage(topUserLanguage);

        // This obtains the language characteristics by providing a BCP47 tag.
        Windows::Globalization::Language exampleLanguage(L"ja");

        // Display the results
        OutputTextBlock().Text(L"User's Preferred Language\n" + ReportLanguageData(userLanguage) +
            L"Example Language by BCP47 tag (ja)\n" + ReportLanguageData(exampleLanguage));
    }
}
