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
#include "Scenario1_LanguageDetection.h"
#include "Scenario1_LanguageDetection.g.cpp"
#include "elsutil.h"
#include <sstream>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_LanguageDetection::Scenario1_LanguageDetection()
    {
        InitializeComponent();
    }

    void Scenario1_LanguageDetection::Go_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto input = TextInput().Text();

        std::wostringstream output;
        ELS::RecognizeTextLanguages(input.c_str(), input.size(), [&](auto lang)
            {
                output << lang << L"\n";
            });

        TextOutput().Text(output.str());
    }
}
