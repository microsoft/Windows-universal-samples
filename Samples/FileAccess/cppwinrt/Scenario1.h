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

#include "Scenario1.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    /// <summary>
    /// Creating a file.
    /// </summary>
    struct Scenario1 : Scenario1T<Scenario1>
    {
        Scenario1();
        fire_and_forget CreateFileButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1 : Scenario1T<Scenario1, implementation::Scenario1>
    {
    };
}
