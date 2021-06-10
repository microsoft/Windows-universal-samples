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
#include "Scenario5_VerifyResponse.h"
#include "Scenario5_VerifyResponse.g.cpp"

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
    Scenario5_VerifyResponse::Scenario5_VerifyResponse()
    {
        InitializeComponent();
    }

    /// <summary>
    /// Click handler for the 'VerifyResponse' button. 
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>

    fire_and_forget Scenario5_VerifyResponse::VerifyResponse_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
        auto lifetime = get_strong();

        if (!ValidateTPMSmartCard())
        {
            m_rootPage.NotifyUser(L"Use Scenario One to create a TPM virtual smart card.", NotifyType::ErrorMessage);
            return;
        }

        auto b = sender.as<Button>();

        try
        {
            SmartCard card = co_await GetSmartCardAsync();
            SmartCardProvisioning provisioning = co_await SmartCardProvisioning::FromSmartCardAsync(card);

            m_rootPage.NotifyUser(L"Verifying smart card response...", NotifyType::StatusMessage);

            // We rely on this object's destructor to clean up the context.
            SmartCardChallengeContext context = co_await provisioning.GetChallengeContextAsync();

            IBuffer response = CalculateChallengeResponse(context.Challenge(), GetAdminKey());
            bool verifyResult = co_await context.VerifyResponseAsync(response);

            m_rootPage.NotifyUser(L"Smart card response verification completed. Result: " + std::to_wstring(verifyResult), NotifyType::StatusMessage);
            // context destructed here
        }
        catch (hresult_error const& ex)
        {
            m_rootPage.NotifyUser(L"Verifying smart card response operation failed with exception: " + ex.message(), NotifyType::ErrorMessage);
        }

        b.IsEnabled(true);
    }
}