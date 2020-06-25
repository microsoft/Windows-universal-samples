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
#include "Scenario3_Transliteration.h"
#include "Scenario3_Transliteration.g.cpp"
#include "elsutil.h"
#include <sstream>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario3_Transliteration::Scenario3_Transliteration()
    {
        InitializeComponent();
    }

    void Scenario3_Transliteration::Go_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto input = TextInput().Text();

        std::wostringstream output;
        ELS::TransliterateFromCyrillicToLatin(input.c_str(), input.size(), [&](auto text, auto length)
            {
                output << std::wstring_view(text, length);
            });

        TextOutput().Text(output.str());
    }
}
