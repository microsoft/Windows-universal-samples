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

#include "Scenario1_Files.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_Files : Scenario1_FilesT<Scenario1_Files>
    {
        Scenario1_Files();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        fire_and_forget ChooseFile_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget ChooseFolder_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget ProtectL1_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget ProtectL2_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget Unprotect_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        winrt::Windows::Security::DataProtection::UserDataProtectionManager userDataProtectionManager{ nullptr };
        winrt::Windows::Storage::IStorageItem selectedItem{ nullptr };

        void UpdateItem(winrt::Windows::Storage::IStorageItem const& item);
        void ReportStatus(winrt::hstring const& operation, winrt::Windows::Security::DataProtection::UserDataStorageItemProtectionStatus status);

    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_Files : Scenario1_FilesT<Scenario1_Files, implementation::Scenario1_Files>
    {
    };
}
