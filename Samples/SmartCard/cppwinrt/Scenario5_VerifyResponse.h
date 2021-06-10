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

#include "Scenario5_VerifyResponse.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario5_VerifyResponse : Scenario5_VerifyResponseT<Scenario5_VerifyResponse>
    {
        SDKTemplate::MainPage m_rootPage{ SDKTemplate::MainPage::Current() };

        Scenario5_VerifyResponse();

        fire_and_forget VerifyResponse_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const&);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario5_VerifyResponse : Scenario5_VerifyResponseT<Scenario5_VerifyResponse, implementation::Scenario5_VerifyResponse>
    {
    };
}