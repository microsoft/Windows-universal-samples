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

#include <string>
#include "SampleConfiguration.h"
#include "Scenario2_ChangePIN.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario2_ChangePIN : Scenario2_ChangePINT<Scenario2_ChangePIN>
    {
        SDKTemplate::MainPage m_rootPage{ SDKTemplate::MainPage::Current() };

        Scenario2_ChangePIN();

        fire_and_forget ChangePin_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const&);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_ChangePIN : Scenario2_ChangePINT<Scenario2_ChangePIN, implementation::Scenario2_ChangePIN>
    {
    };
}