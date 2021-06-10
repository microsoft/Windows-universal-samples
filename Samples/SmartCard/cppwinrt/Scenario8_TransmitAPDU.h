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

#include "Scenario8_TransmitAPDU.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario8_TransmitAPDU : Scenario8_TransmitAPDUT<Scenario8_TransmitAPDU>
    {
        SDKTemplate::MainPage m_rootPage{ SDKTemplate::MainPage::Current() };

        Scenario8_TransmitAPDU();

        fire_and_forget Transmit_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const&);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario8_TransmitAPDU : Scenario8_TransmitAPDUT<Scenario8_TransmitAPDU, implementation::Scenario8_TransmitAPDU>
    {
    };
}