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
#include "Scenario6_DeleteTPM.h"
#include "Scenario6_DeleteTPM.g.cpp"

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Windows::Devices::SmartCards;
    using namespace Windows::UI::Xaml;
    using namespace Windows::UI::Xaml::Controls;
}

namespace winrt::SDKTemplate::implementation
{
    Scenario6_DeleteTPM::Scenario6_DeleteTPM()
    {
        InitializeComponent();
    }

    /// <summary>
    /// Click handler for the 'Delete' button.
    /// </summary>
    /// <param name="sender"></param>
    fire_and_forget Scenario6_DeleteTPM::Delete_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();
        if (!ValidateTPMSmartCard())
        {
            m_rootPage.NotifyUser(L"Use Scenario One to create a TPM virtual smart card.", NotifyType::ErrorMessage);
            co_return;
        }

        auto b = sender.as<Button>();
        b.IsEnabled(false);
        m_rootPage.NotifyUser(L"Deleting the TPM virtual smart card...", NotifyType::ErrorMessage);

        try
        {
            SmartCard card = co_await GetSmartCardAsync();

            // The following two lines are not directly related to TPM virtual
            // smart card creation, but are used to demonstrate how to handle
            // CardRemoved events by registering an event handler with a
            // SmartCardReader object.  Since we are using a TPM virtual smart
            // card in this case, the card cannot actually be added to or
            // removed from the reader, but a CardRemoved event will fire
            // when the reader and card are deleted.
            //
            // We must retain a reference to the SmartCardReader object to
            // which we are adding the event handler.  We use += to add the
            // HandleCardRemoved method as an event handler.  The function
            // will be automatically boxed in a TypedEventHandler, but
            // the function signature match the template arguments for
            // the specific event - in this case,
            // <SmartCardReader, CardRemovedEventArgs>
            m_reader = card.Reader();
            m_cardRemovedToken = m_reader.CardRemoved(winrt::auto_revoke, { get_weak(), &Scenario6_DeleteTPM::HandleCardRemoved });

            bool result = co_await SmartCardProvisioning::RequestVirtualSmartCardDeletionAsync(card);

            if (result)
            {
                m_rootPage.NotifyUser(L"TPM virtual smart card deletion completed.", NotifyType::StatusMessage);
                SetSmartCardReaderDeviceId(winrt::hstring{});
            }
            else
            {
                m_rootPage.NotifyUser(L"TPM virtual smart card deletion was canceled by the user.", NotifyType::StatusMessage);
            }
        }
        catch (hresult_error const& ex)
        {
            m_rootPage.NotifyUser(L"TPM virtual smartcard deletion failed with exception: " + ex.message(), NotifyType::ErrorMessage);
        }

        b.IsEnabled(true);
    }

    void Scenario6_DeleteTPM::HandleCardRemoved(Windows::Devices::SmartCards::SmartCardReader const& evtReader, Windows::Devices::SmartCards::CardRemovedEventArgs const&)
    {
        // Note that this event is raised from a background thread.
        // However, the NotifyUser method is safe to call from background threads.
        m_rootPage.NotifyUser(L"Card removed from reader " + evtReader.Name() + L".", NotifyType::StatusMessage);
    }
}
