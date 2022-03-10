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
#include "Scenario5_RestoreScrollPosition.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario5_RestoreScrollPosition : Scenario5_RestoreScrollPositionT<Scenario5_RestoreScrollPosition>
    {
        Scenario5_RestoreScrollPosition();

        auto _items() { return items; }
        fire_and_forget Page_Loaded(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs const&);
        void ItemsListView_ContainerContentChanging(Windows::UI::Xaml::Controls::ListViewBase const&, Windows::UI::Xaml::Controls::ContainerContentChangingEventArgs const& args);
        void ItemsListView_ItemClick(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        Windows::Foundation::Collections::IObservableVector<SDKTemplate::Item> items = Item::GetItems(300);

        // Static persisted variables are used by ListViewPersistenceHelper
        // We need to save the item container height if the items have variable heights. If all items have a constant fixed height, you can manually 
        // set the height to the fixed value in ItemsListView_ContainerContentChanging
        static double _persistedItemContainerHeight;
        static hstring _persistedItemKey;
        static hstring _persistedPosition;

        Windows::Foundation::IAsyncOperation<Windows::Foundation::IInspectable> GetItem(hstring const& key);
        hstring GetKey(IInspectable const& object);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario5_RestoreScrollPosition : Scenario5_RestoreScrollPositionT<Scenario5_RestoreScrollPosition, implementation::Scenario5_RestoreScrollPosition>
    {
    };
}
