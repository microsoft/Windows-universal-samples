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

#include "Scenario6_DeleteTPM.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario6_DeleteTPM : Scenario6_DeleteTPMT<Scenario6_DeleteTPM>
    {
        Scenario6_DeleteTPM();

        fire_and_forget Delete_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        SDKTemplate::MainPage m_rootPage{ SDKTemplate::MainPage::Current() };

        Windows::Devices::SmartCards::SmartCardReader m_reader{ nullptr };
        Windows::Devices::SmartCards::SmartCardReader::CardRemoved_revoker m_cardRemovedToken;

        void HandleCardRemoved(Windows::Devices::SmartCards::SmartCardReader const& evtReader, Windows::Devices::SmartCards::CardRemovedEventArgs const&);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario6_DeleteTPM : Scenario6_DeleteTPMT<Scenario6_DeleteTPM, implementation::Scenario6_DeleteTPM>
    {
    };
}
