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

#include "Scenario6.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    /// <summary>
    /// Displaying file properties.
    /// </summary>
    struct Scenario6 : Scenario6T<Scenario6>
    {
        Scenario6();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        fire_and_forget ShowPropertiesButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario6 : Scenario6T<Scenario6, implementation::Scenario6>
    {
    };
}
