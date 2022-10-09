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
#include "Scenario8_TransmitAPDU.h"
#include "Scenario8_TransmitAPDU.g.cpp"

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Windows::Security::Cryptography;
    using namespace Windows::Devices::SmartCards;
    using namespace Windows::Devices::Enumeration;
    using namespace Windows::UI::Xaml;
    using namespace Windows::UI::Xaml::Controls;
    using namespace Windows::Storage::Streams;
}

namespace winrt::SDKTemplate::implementation
{
    Scenario8_TransmitAPDU::Scenario8_TransmitAPDU()
    {
        InitializeComponent();
    }

    /// <summary>
    /// Click handler for the 'TransmitAPDU' button.
    /// </summary>
    /// <param name="sender"></param>
    fire_and_forget Scenario8_TransmitAPDU::Transmit_Click(IInspectable const& sender, RoutedEventArgs const&)
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

            // We rely on this object's destructor to clean up the connection.
            SmartCardConnection connection = co_await card.ConnectAsync();

            // Read EF.ATR file
            // The command is meant specifically for GIDS cards 
            // (such as TPM VSCs), and will fail on other types.
            static constexpr uint8_t readEfAtrBytes[] = { 0x00, 0xCB, 0x2F, 0x01, 0x02, 0x5C, 0x00, 0xFF };
            IBuffer readEfAtr = CryptographicBuffer::CreateFromByteArray(readEfAtrBytes);
            IBuffer result = co_await connection.TransmitAsync(readEfAtr);

            m_rootPage.NotifyUser(L"Response: " + CryptographicBuffer::EncodeToHexString(result), NotifyType::StatusMessage);
            // connection destructed here
        }
        catch (hresult_error const& ex)
        {
            m_rootPage.NotifyUser(L"Transmitting APDU to card failed with exception: " + ex.message(), NotifyType::ErrorMessage);
        }

        b.IsEnabled(true);
    }
}