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

#include "Scenario2_Manage.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario2_Manage : Scenario2_ManageT<Scenario2_Manage>
    {
        Scenario2_Manage();

        void RetrieveCredentials_Click(Windows::Foundation::IInspectable const&, Windows::Foundation::IInspectable const&);
        void RevealPasswords_Click(Windows::Foundation::IInspectable const&, Windows::Foundation::IInspectable const&);
        void RemoveCredentials_Click(Windows::Foundation::IInspectable const&, Windows::Foundation::IInspectable const&);

    private:
        SDKTemplate::MainPage rootPage{ SDKTemplate::implementation::MainPage::Current() };
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_Manage : Scenario2_ManageT<Scenario2_Manage, implementation::Scenario2_Manage>
    {
    };
}

