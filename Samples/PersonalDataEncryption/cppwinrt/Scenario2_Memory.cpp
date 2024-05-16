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
#include "Scenario2_Memory.h"
#include "Scenario2_Memory.g.cpp"

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Security::Cryptography;
using namespace winrt::Windows::Security::DataProtection;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_Memory::Scenario2_Memory()
    {
        InitializeComponent();
    }

    void Scenario2_Memory::OnNavigatedTo(NavigationEventArgs const&)
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

    fire_and_forget Scenario2_Memory::ProtectL1_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();
        hstring text = DataTextBox().Text();
        if (text.empty())
        {
            // Cannot protect a zero-length buffer.
            rootPage.NotifyUser(L"Please enter data to protect", NotifyType::ErrorMessage);
            co_return;
        }

        IBuffer buffer = CryptographicBuffer::ConvertStringToBinary(text, BinaryStringEncoding::Utf8);

        IBuffer result = co_await userDataProtectionManager.ProtectBufferAsync(buffer, UserDataAvailability::AfterFirstUnlock);
        ReportProtectedBuffer(result);
    }

    fire_and_forget Scenario2_Memory::ProtectL2_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();
        hstring text = DataTextBox().Text();
        if (text.empty())
        {
            // Cannot protect a zero-length buffer.
            rootPage.NotifyUser(L"Please enter data to protect", NotifyType::ErrorMessage);
            co_return;
        }

        IBuffer buffer = CryptographicBuffer::ConvertStringToBinary(text, BinaryStringEncoding::Utf8);

        IBuffer result = co_await userDataProtectionManager.ProtectBufferAsync(buffer, UserDataAvailability::WhileUnlocked);
        ReportProtectedBuffer(result);
    }

    void Scenario2_Memory::ReportProtectedBuffer(IBuffer const& buffer)
    {
        protectedBuffer = buffer;
        ProtectedDataTextBlock().Text(CryptographicBuffer::EncodeToHexString(buffer));
        UnprotectPanel().Visibility(Visibility::Visible);
        rootPage.NotifyUser(L"", NotifyType::StatusMessage);
    }

    fire_and_forget Scenario2_Memory::Unprotect_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();
        UnprotectedDataTextBlock().Text(L"");
        UserDataBufferUnprotectResult result = co_await userDataProtectionManager.UnprotectBufferAsync(protectedBuffer);
        switch (result.Status())
        {
        case UserDataBufferUnprotectStatus::Succeeded:
            rootPage.NotifyUser(L"Unprotected successfully", NotifyType::StatusMessage);
            UnprotectedDataTextBlock().Text(CryptographicBuffer::ConvertBinaryToString(BinaryStringEncoding::Utf8, result.UnprotectedBuffer()));
            break;

        case UserDataBufferUnprotectStatus::Unavailable:
            rootPage.NotifyUser(L"Unable to unprotect: Unavailable", NotifyType::ErrorMessage);
            break;

        default:
            rootPage.NotifyUser(L"Unable to unprotect: Unknown error", NotifyType::ErrorMessage);
            break;
        }
    }
}