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
#include "Scenario1_ProvisionTPM.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_ProvisionTPM : Scenario1_ProvisionTPMT<Scenario1_ProvisionTPM>
    {
        Scenario1_ProvisionTPM();

        fire_and_forget Create_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        SDKTemplate::MainPage m_rootPage{ SDKTemplate::MainPage::Current() };

        Windows::Devices::SmartCards::SmartCardReader m_reader{ nullptr };
        Windows::Devices::SmartCards::SmartCardReader::CardAdded_revoker m_cardAddedToken;

        void HandleCardAdded(Windows::Devices::SmartCards::SmartCardReader const&, Windows::Devices::SmartCards::CardAddedEventArgs const&);

        Windows::Devices::SmartCards::SmartCardPinPolicy ParsePinPolicy();
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_ProvisionTPM : Scenario1_ProvisionTPMT<Scenario1_ProvisionTPM, implementation::Scenario1_ProvisionTPM>
    {
    };
};