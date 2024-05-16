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

#include "pch.h"
#include "Scenario1_Files.h"
#include "Scenario1_Files.g.cpp"

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Security::DataProtection;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Pickers;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_Files::Scenario1_Files()
    {
        InitializeComponent();
    }

    void Scenario1_Files::OnNavigatedTo(NavigationEventArgs const&)
    {
        userDataProtectionManager = UserDataProtectionManager::TryGetDefault();
        if (userDataProtectionManager)
        {
            AvailablePanel().Visibility(Visibility::Visible);
            UnavailablePanel().Visibility(Visibility::Collapsed);
        }
        else
        {
            AvailablePanel().Visibility(Visibility::Collapsed);
            UnavailablePanel().Visibility(Visibility::Visible);
        }
    }

    fire_and_forget Scenario1_Files::ChooseFile_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();
        FileOpenPicker picker;
        picker.FileTypeFilter().Append(L"*");
        UpdateItem(co_await picker.PickSingleFileAsync());
    }

    fire_and_forget Scenario1_Files::ChooseFolder_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();
        FolderPicker picker;
        UpdateItem(co_await picker.PickSingleFolderAsync());
    }

    void Scenario1_Files::UpdateItem(IStorageItem const& item)
    {
        selectedItem = item;
        if (selectedItem != nullptr)
        {
            ItemOperationsPanel().Visibility(Visibility::Visible);
            ItemNameBlock().Text(item.Path());
        }
        else
        {
            ItemOperationsPanel().Visibility(Visibility::Collapsed);
        }
    }

    fire_and_forget Scenario1_Files::ProtectL1_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();
        UserDataStorageItemProtectionStatus status = co_await userDataProtectionManager.ProtectStorageItemAsync(selectedItem, UserDataAvailability::AfterFirstUnlock);
        ReportStatus(L"Protect L1", status);
    }

    fire_and_forget Scenario1_Files::ProtectL2_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();
        UserDataStorageItemProtectionStatus status = co_await userDataProtectionManager.ProtectStorageItemAsync(selectedItem, UserDataAvailability::WhileUnlocked);
        ReportStatus(L"Protect L2", status);
    }

    fire_and_forget Scenario1_Files::Unprotect_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();
        UserDataStorageItemProtectionStatus status = co_await userDataProtectionManager.ProtectStorageItemAsync(selectedItem, UserDataAvailability::Always);
        ReportStatus(L"Protect L2", status);
    }

    void Scenario1_Files::ReportStatus(hstring const& operation, UserDataStorageItemProtectionStatus status)
    {
        switch (status)
        {
        case UserDataStorageItemProtectionStatus::Succeeded:
            rootPage.NotifyUser(operation + L" succeeded", NotifyType::StatusMessage);
            break;

        case UserDataStorageItemProtectionStatus::NotProtectable:
            rootPage.NotifyUser(operation + L" failed: Not protectable", NotifyType::ErrorMessage);
            break;

        case UserDataStorageItemProtectionStatus::DataUnavailable:
            rootPage.NotifyUser(operation + L" failed: Data unavailable", NotifyType::ErrorMessage);
            break;

        default:
            rootPage.NotifyUser(operation + L" failed: Unknown failure", NotifyType::ErrorMessage);
            break;
        }
    }
}
