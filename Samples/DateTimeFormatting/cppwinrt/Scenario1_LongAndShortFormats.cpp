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
#include "Scenario1_LongAndShortFormats.h"
#include "Scenario1_LongAndShortFormats.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_LongAndShortFormats::Scenario1_LongAndShortFormats()
    {
        InitializeComponent();
    }

    void Scenario1_LongAndShortFormats::Display_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // This scenario uses the Windows.Globalization.DateTimeFormatting.DateTimeFormatter class
        // in order to display dates and times using basic formatters.

        //Get the current application context language
        hstring currentLanguage = ApplicationLanguages::Languages().GetAt(0);

        // Formatters for dates and times
        std::array dateFormatters{
            DateTimeFormatter(L"shortdate"),
            DateTimeFormatter(L"longdate"),
            DateTimeFormatter(L"shorttime"),
            DateTimeFormatter(L"longtime"),
        };

        // Obtain the date that will be formatted.
        DateTime dateToFormat = clock::now();

        // Keep the results here
        std::wostringstream results;
        results << L"Current application context language: " << std::wstring_view(currentLanguage) << std::endl << std::endl;

        // Generate the results.
        for (auto&& formatter : dateFormatters)
        {
            // Perform the actual formatting. 
            results << std::wstring_view(formatter.Template()) << L": " << std::wstring_view(formatter.Format(dateToFormat)) << std::endl;
        }

        // Display the results.
        OutputTextBlock().Text(results.str());
    }
}
