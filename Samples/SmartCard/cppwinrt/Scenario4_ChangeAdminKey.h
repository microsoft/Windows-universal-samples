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

#include "Scenario4_ChangeAdminKey.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario4_ChangeAdminKey : Scenario4_ChangeAdminKeyT<Scenario4_ChangeAdminKey>
    {
        SDKTemplate::MainPage m_rootPage{ SDKTemplate::MainPage::Current() };

        Scenario4_ChangeAdminKey();

        fire_and_forget ChangeAdminKey_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const&);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario4_ChangeAdminKey : Scenario4_ChangeAdminKeyT<Scenario4_ChangeAdminKey, implementation::Scenario4_ChangeAdminKey>
    {
    };
}