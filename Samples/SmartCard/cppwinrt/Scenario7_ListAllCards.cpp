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
#include "Scenario7_ListAllCards.h"
#include "Scenario7_ListAllCards.g.cpp"
#include "SmartCardListItem.g.cpp"

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Windows::Devices::SmartCards;
    using namespace Windows::Devices::Enumeration;
    using namespace Windows::UI::Xaml;
    using namespace Windows::UI::Xaml::Controls;
}

namespace winrt::SDKTemplate::implementation
{
    Scenario7_ListAllCards::Scenario7_ListAllCards()
    {
        InitializeComponent();
    }

    /// <summary>
    /// Click handler for the 'ListSmartCard' button.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>

    fire_and_forget Scenario7_ListAllCards::ListSmartCard_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();
        auto b = sender.as<Button>();
        b.IsEnabled(false);
        try
        {
            m_rootPage.NotifyUser(L"Enumerating smart cards...", NotifyType::StatusMessage);

            // This list will be bound to our ItemListView once it has been
            // filled with SmartCardListItems.  The SmartCardListItem class
            // is defined in our Project.idl, and describes a reader/card pair with a
            // reader name and a card name.
            auto cardItems = winrt::single_threaded_observable_vector<SDKTemplate::SmartCardListItem>();

            // First we get the device selector for smart card readers using
            // the static GetDeviceSelector method of the SmartCardReader
            // class.  The selector is a string which describes a class of
            // devices to query for, and is used as the argument to
            // DeviceInformation.FindAllAsync.  GetDeviceSelector is
            // overloaded so that you can provide a SmartCardReaderKind
            // to specify if you are only interested in a particular type
            // of card/reader (e.g. TPM virtual smart card.)  In this case
            // we will list all cards and readers.
            hstring selector = SmartCardReader::GetDeviceSelector();
            DeviceInformationCollection devices = co_await DeviceInformation::FindAllAsync(selector);

            // DeviceInformation.FindAllAsync gives us a
            // DeviceInformationCollection, which is essentially a list
            // of DeviceInformation objects.  We must iterate through that
            // list and instantiate SmartCardReader objects from the
            // DeviceInformation objects.
            for (DeviceInformation const& device : devices)
            {
                SmartCardReader reader = co_await SmartCardReader::FromIdAsync(device.Id());

                // For each reader, we want to find all the cards associated
                // with it.  Then we will create a SmartCardListItem for
                // each (reader, card) pair.
                auto cards = co_await reader.FindAllCardsAsync();

                for (SmartCard const& card : cards)
                {
                    SmartCardProvisioning provisioning = co_await SmartCardProvisioning::FromSmartCardAsync(card);

                    auto provisionedName = co_await provisioning.GetNameAsync();
                    auto item = SDKTemplate::SmartCardListItem{ card.Reader().Name(), provisionedName };
                    cardItems.Append(item);
                }
            }

            // Bind the source of ItemListView to our SmartCardListItem list.
            ItemListView().ItemsSource(cardItems);

            m_rootPage.NotifyUser(L"Enumerating smart cards completed.", NotifyType::StatusMessage);
        }
        catch (hresult_error const& ex)
        {
            m_rootPage.NotifyUser(L"Enumerating smart cards failed with exception: " + ex.message(), NotifyType::ErrorMessage);
        }

        b.IsEnabled(true);
    }
}
