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

#include "Scenario2.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    /// <summary>
    /// Getting a file's parent folder.
    /// </summary>
    struct Scenario2 : Scenario2T<Scenario2>
    {
        Scenario2();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        fire_and_forget GetParent_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2 : Scenario2T<Scenario2, implementation::Scenario2>
    {
    };
}
