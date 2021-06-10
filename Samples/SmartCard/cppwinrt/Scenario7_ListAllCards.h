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
#include "Scenario7_ListAllCards.g.h"
#include "SmartCardListItem.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario7_ListAllCards : Scenario7_ListAllCardsT<Scenario7_ListAllCards>
    {
        SDKTemplate::MainPage m_rootPage{ SDKTemplate::MainPage::Current() };

        Scenario7_ListAllCards();

        fire_and_forget ListSmartCard_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const&);
    };

    struct SmartCardListItem : SmartCardListItemT<SmartCardListItem>
    {
        SmartCardListItem(hstring const& readerName, hstring const& cardName) :
            m_readerName(readerName),
            m_cardName(cardName)
        {
        }

        hstring ReaderName()
        {
            return m_readerName;
        }

        hstring CardName()
        {
            return m_cardName;
        }

    private:
        hstring m_readerName;
        hstring m_cardName;
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario7_ListAllCards : Scenario7_ListAllCardsT<Scenario7_ListAllCards, implementation::Scenario7_ListAllCards>
    {
    };

    struct SmartCardListItem : SmartCardListItemT<SmartCardListItem, implementation::SmartCardListItem>
    {
    };
}