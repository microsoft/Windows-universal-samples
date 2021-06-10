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
#include "Scenario4_ChangeAdminKey.h"
#include "Scenario4_ChangeAdminKey.g.cpp"

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Windows::Storage::Streams;
    using namespace Windows::Devices::SmartCards;
    using namespace Windows::Security::Cryptography;
    using namespace Windows::UI::Xaml;
    using namespace Windows::UI::Xaml::Controls;
}

namespace winrt::SDKTemplate::implementation
{
    Scenario4_ChangeAdminKey::Scenario4_ChangeAdminKey()
    {
        InitializeComponent();
    }

    /// <summary>
    /// Click handler for the 'ChangeAdminKey' button.
    /// </summary>
    /// <param name="sender"></param>
    fire_and_forget Scenario4_ChangeAdminKey::ChangeAdminKey_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        if (!ValidateTPMSmartCard())
        {
            m_rootPage.NotifyUser(L"Use Scenario One to create a TPM virtual smart card.", NotifyType::ErrorMessage);
            co_return;
        }

        auto b = sender.as<Windows::UI::Xaml::Controls::Button>();
        b.IsEnabled(false);

        try
        {
            IBuffer newadminkey = CryptographicBuffer::GenerateRandom(ADMIN_KEY_LENGTH_IN_BYTES);

            SmartCard card = co_await GetSmartCardAsync();
            SmartCardProvisioning provisioning = co_await SmartCardProvisioning::FromSmartCardAsync(card);

            m_rootPage.NotifyUser(L"Changing smart card admin key...", NotifyType::StatusMessage);

            // We rely on this object's destructor to clean up the context.
            SmartCardChallengeContext context = co_await provisioning.GetChallengeContextAsync();

            IBuffer response = CalculateChallengeResponse(context.Challenge(), GetAdminKey());
            co_await context.ChangeAdministrativeKeyAsync(response, newadminkey);
            SetAdminKey(newadminkey);

            m_rootPage.NotifyUser(L"Smart card change admin key operation completed.", NotifyType::StatusMessage);
            // context destructed here
        }
        catch (hresult_error const& ex)
        {
            m_rootPage.NotifyUser(L"Changing smart card admin key operation failed with exception: " + ex.message(), NotifyType::ErrorMessage);
        }

        b.IsEnabled(false);
    }

}
