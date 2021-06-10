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

#pragma once;

#include "Scenario3_ResetPIN.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario3_ResetPIN : Scenario3_ResetPINT<Scenario3_ResetPIN>
    {
        SDKTemplate::MainPage m_rootPage{ SDKTemplate::MainPage::Current() };

        Scenario3_ResetPIN();

        fire_and_forget ResetPin_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const&);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario3_ResetPIN : Scenario3_ResetPINT<Scenario3_ResetPIN, implementation::Scenario3_ResetPIN>
    {
    };
}