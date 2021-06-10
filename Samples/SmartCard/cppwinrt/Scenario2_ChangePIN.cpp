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
#include "Scenario2_ChangePIN.h"
#include "Scenario2_ChangePIN.g.cpp"

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Windows::UI::Xaml;
    using namespace Windows::UI::Xaml::Controls;
    using namespace Windows::Devices::SmartCards;
}

namespace winrt::SDKTemplate::implementation
{
    Scenario2_ChangePIN::Scenario2_ChangePIN()
    {
        InitializeComponent();
    }

    /// <summary>
    /// Click handler for the 'ChangePin' button.
    /// </summary>
    /// <param name="sender"></param>
    fire_and_forget Scenario2_ChangePIN::ChangePin_Click(IInspectable const& sender, RoutedEventArgs const&)
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
            SmartCardProvisioning provisioning = co_await Windows::Devices::SmartCards::SmartCardProvisioning::FromSmartCardAsync(card);

            m_rootPage.NotifyUser(L"Changing smart card PIN...", NotifyType::StatusMessage);

            bool result = co_await provisioning.RequestPinChangeAsync();

            if (result)
            {
                m_rootPage.NotifyUser(L"Smart card change PIN operation completed.", NotifyType::StatusMessage);
            }
            else
            {
                m_rootPage.NotifyUser(L"Smart card change PIN operation was canceled by the user.", NotifyType::StatusMessage);
            }
        }
        catch (hresult_error const& ex)
        {
            m_rootPage.NotifyUser(L"Changing smart card PIN failed with exception: " + ex.message(), NotifyType::ErrorMessage);
        }

        b.IsEnabled(true);
    }
}