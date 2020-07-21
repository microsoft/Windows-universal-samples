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
#include "Scenario2_Lang.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

Scenario2_Lang::Scenario2_Lang()
{
    InitializeComponent();
}

String^ Scenario2_Lang::ReportLanguageData(Windows::Globalization::Language^ lang)
{
    return "Display Name: " + lang->DisplayName + "\n" +
        "Language Tag: " + lang->LanguageTag + "\n" +
        "Native Name: " + lang->NativeName + "\n" +
        "Script Code: " + lang->Script + "\n\n";

}
void Scenario2_Lang::ShowResults()
{
    // This scenario uses the Windows.System.UserProfile.GlobalizationPreferences class to
    // obtain the user's preferred language characteristics.
    String^ topUserLanguage = Windows::System::UserProfile::GlobalizationPreferences::Languages->GetAt(0);
    auto userLanguage = ref new Windows::Globalization::Language(topUserLanguage);

    // This obtains the language characteristics by providing a BCP47 tag.
    auto exampleLanguage = ref new Windows::Globalization::Language("ja");

    // Display the results
    OutputTextBlock->Text = "User's Preferred Language\n" + ReportLanguageData(userLanguage) +
        "Example Language by BCP47 tag (ja)\n" + ReportLanguageData(exampleLanguage);
}