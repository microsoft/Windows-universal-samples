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

#include "Scenario2_RequestMany.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario2_RequestMany : Scenario2_RequestManyT<Scenario2_RequestMany>
    {
        Scenario2_RequestMany();

        fire_and_forget RequestAccessButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_RequestMany : Scenario2_RequestManyT<Scenario2_RequestMany, implementation::Scenario2_RequestMany>
    {
    };
}
