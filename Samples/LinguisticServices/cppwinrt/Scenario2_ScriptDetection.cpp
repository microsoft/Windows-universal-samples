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
#include "Scenario2_ScriptDetection.h"
#include "Scenario2_ScriptDetection.g.cpp"
#include "elsutil.h"
#include <sstream>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_ScriptDetection::Scenario2_ScriptDetection()
    {
        InitializeComponent();
    }

    void Scenario2_ScriptDetection::Go_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto input = TextInput().Text();

        std::wostringstream output;
        ELS::RecognizeTextScripts(input.c_str(), input.size(), [&](auto first, auto last, auto script)
            {
                output << L"Range from " << first << L" to " << last << L": " << script << L" script\n";
            });

        TextOutput().Text(output.str());
    }
}
