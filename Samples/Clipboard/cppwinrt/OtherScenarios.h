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

#include "OtherScenarios.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct OtherScenarios : OtherScenariosT<OtherScenarios>
    {
        OtherScenarios();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        void ShowFormatButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void EmptyClipboardButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void RegisterClipboardContentChanged_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void ClearOutputButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        static bool registerContentChanged;

        void ClearOutput();
        void DisplayFormats();
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct OtherScenarios : OtherScenariosT<OtherScenarios, implementation::OtherScenarios>
    {
    };
}
