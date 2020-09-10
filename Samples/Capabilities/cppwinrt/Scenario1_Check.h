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

#include "Scenario1_Check.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_Check : Scenario1_CheckT<Scenario1_Check>
    {
        Scenario1_Check();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);

        fire_and_forget RequestAccessButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        Windows::Security::Authorization::AppCapabilityAccess::AppCapability locationCapability{ nullptr };
        event_token accessChangedToken;

        fire_and_forget OnCapabilityAccessChanged(Windows::Security::Authorization::AppCapabilityAccess::AppCapability const&, Windows::Foundation::IInspectable const&);
        Windows::Foundation::IAsyncAction ShowLocationAsync();
        fire_and_forget UpdateCapabilityStatus();
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_Check : Scenario1_CheckT<Scenario1_Check, implementation::Scenario1_Check>
    {
    };
}
