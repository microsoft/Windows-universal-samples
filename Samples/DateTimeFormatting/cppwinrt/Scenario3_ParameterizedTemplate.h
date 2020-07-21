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

#pragma once

#include "Scenario3_ParameterizedTemplate.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario3_ParameterizedTemplate : Scenario3_ParameterizedTemplateT<Scenario3_ParameterizedTemplate>
    {
        Scenario3_ParameterizedTemplate();

        void Display_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario3_ParameterizedTemplate : Scenario3_ParameterizedTemplateT<Scenario3_ParameterizedTemplate, implementation::Scenario3_ParameterizedTemplate>
    {
    };
}
