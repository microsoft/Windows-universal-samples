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

#include "Scenario1_FindUsers.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_FindUsers : Scenario1_FindUsersT<Scenario1_FindUsers>
    {
        Scenario1_FindUsers();

        fire_and_forget OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        fire_and_forget ShowProperties();

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_FindUsers : Scenario1_FindUsersT<Scenario1_FindUsers, implementation::Scenario1_FindUsers>
    {
    };
}
