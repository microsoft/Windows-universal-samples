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
#include "Scenario3_ResetPIN.h"
#include "Scenario3_ResetPIN.g.cpp"

namespace winrt
{
    using namespace Windows::Devices::SmartCards;
    using namespace Windows::Foundation;
    using namespace Windows::Storage::Streams;
    using namespace Windows::UI::Xaml;
    using namespace Windows::UI::Xaml::Controls;
}

namespace winrt::SDKTemplate::implementation
{
    Scenario3_ResetPIN::Scenario3_ResetPIN()
    {
        InitializeComponent();
    }

    /// <summary>
    /// Click handler for the 'ResetPin' button. 
    /// </summary>
    /// <param name="sender"></param>
    fire_and_forget Scenario3_ResetPIN::ResetPin_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        if (!ValidateTPMSmartCard())
        {
            m_rootPage.NotifyUser(L"Use Scenario One to create a TPM virtual smart card.", NotifyType::ErrorMessage);
            co_return;
        }

        auto b = sender.as<Button>();
        b.IsEnabled(false);

        try
        {
            SmartCard card = co_await GetSmartCardAsync();
            SmartCardProvisioning provisioning = co_await SmartCardProvisioning::FromSmartCardAsync(card);

            m_rootPage.NotifyUser(L"Resetting smart card PIN...", NotifyType::StatusMessage);

            // When requesting a PIN reset, a SmartCardPinResetHandler must be
            // provided as an argument.  This handler must use the challenge
            // it receives and the card's admin key to calculate and set the
            // response.
            auto resetHandler = [this, self = get_strong()](SmartCardProvisioning const&, SmartCardPinResetRequest const& request)
            {
                SmartCardPinResetDeferral deferral = request.GetDeferral();
                IBuffer response = CalculateChallengeResponse(request.Challenge(), GetAdminKey());
                request.SetResponse(response);
            };

            bool result = co_await provisioning.RequestPinResetAsync(resetHandler);

            if (result)
            {
                m_rootPage.NotifyUser(L"Smart card PIN reset operation completed.", NotifyType::StatusMessage);
            }
            else
            {
                m_rootPage.NotifyUser(L"Smart card PIN reset operation was canceled by the user.", NotifyType::StatusMessage);
            }
        }
        catch (hresult_error const& ex)
        {
            m_rootPage.NotifyUser(L"Resetting smart card PIN failed with exception: " + ex.message(), NotifyType::ErrorMessage);
        }

        b.IsEnabled(true);
    }
}