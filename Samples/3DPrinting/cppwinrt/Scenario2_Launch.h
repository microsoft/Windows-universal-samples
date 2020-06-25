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

#include "Scenario2_Launch.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario2_Launch : Scenario2_LaunchT<Scenario2_Launch>
    {
        Scenario2_Launch();

        fire_and_forget CheckIf3DBuilderIsInstalled_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget LaunchFileIn3DBuilder_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ SDKTemplate::implementation::MainPage::Current() };
        static constexpr wchar_t PackageFamilyName3DBuilder[] = L"Microsoft.3DBuilder_8wekyb3d8bbwe";
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_Launch : Scenario2_LaunchT<Scenario2_Launch, implementation::Scenario2_Launch>
    {
    };
}
