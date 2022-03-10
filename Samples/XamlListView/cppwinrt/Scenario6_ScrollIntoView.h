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
#include "Item.h"
#include "Scenario6_ScrollIntoView.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario6_ScrollIntoView : Scenario6_ScrollIntoViewT<Scenario6_ScrollIntoView>
    {
        Scenario6_ScrollIntoView();

        auto _items() { return items; }
        void Scroll_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        Windows::Foundation::Collections::IObservableVector<SDKTemplate::Item> items = Item::GetItems(300);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario6_ScrollIntoView : Scenario6_ScrollIntoViewT<Scenario6_ScrollIntoView, implementation::Scenario6_ScrollIntoView>
    {
    };
}
