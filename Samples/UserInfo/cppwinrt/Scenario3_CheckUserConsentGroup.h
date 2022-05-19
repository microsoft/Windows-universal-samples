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

#include "Scenario3_CheckUserConsentGroup.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario3_CheckUserConsentGroup : Scenario3_CheckUserConsentGroupT<Scenario3_CheckUserConsentGroup>
    {
        Scenario3_CheckUserConsentGroup();

        fire_and_forget OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        fire_and_forget ShowConsentGroups(IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };

        static hstring EvaluateConsentResult(Windows::System::UserAgeConsentResult consentResult);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario3_CheckUserConsentGroup : Scenario3_CheckUserConsentGroupT <Scenario3_CheckUserConsentGroup, implementation::Scenario3_CheckUserConsentGroup>
    {
    };
}
