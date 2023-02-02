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
#include "Scenario6_ClearScenario.h"
#include "Scenario6_ClearScenario.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::Storage;

namespace winrt::SDKTemplate::implementation
{
    Scenario6_ClearScenario::Scenario6_ClearScenario()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario6_ClearScenario::Clear_Click(IInspectable const&, RoutedEventArgs const&)
    {
        try
        {
            co_await ApplicationData::Current().ClearAsync();
            OutputTextBlock().Text(L"ApplicationData has been cleared.  Visit the other scenarios to see that their data has been cleared.");
        }
        catch (...)
        {
            OutputTextBlock().Text(L"Unable to clear settings. This can happen when files are in use.");
        }
    }
}

