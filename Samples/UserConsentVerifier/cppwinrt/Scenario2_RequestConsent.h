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

#include "Scenario2_RequestConsent.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario2_RequestConsent : Scenario2_RequestConsentT<Scenario2_RequestConsent>
    {
        Scenario2_RequestConsent();

        fire_and_forget RequestConsent_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);        

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_RequestConsent : Scenario2_RequestConsentT<Scenario2_RequestConsent, implementation::Scenario2_RequestConsent>
    {
    };
}
