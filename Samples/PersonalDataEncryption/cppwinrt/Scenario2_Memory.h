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

#include "Scenario2_Memory.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario2_Memory : Scenario2_MemoryT<Scenario2_Memory>
    {
        Scenario2_Memory();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        fire_and_forget ProtectL1_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget ProtectL2_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget Unprotect_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        winrt::Windows::Security::DataProtection::UserDataProtectionManager userDataProtectionManager{ nullptr };
        winrt::Windows::Storage::Streams::IBuffer protectedBuffer{ nullptr };

        void ReportProtectedBuffer(Windows::Storage::Streams::IBuffer const& buffer);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_Memory : Scenario2_MemoryT<Scenario2_Memory, implementation::Scenario2_Memory>
    {
    };
}
