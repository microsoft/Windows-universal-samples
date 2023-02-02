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
#include "EdgeTappedListView.h"
#include "Scenario4_EdgeTappedListView.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario4_EdgeTappedListView : Scenario4_EdgeTappedListViewT<Scenario4_EdgeTappedListView>
    {
        Scenario4_EdgeTappedListView();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);

        void SelectItems(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void AddItem(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void RemoveItem(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void CancelSelection(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void OnSelectionChanged(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Controls::SelectionChangedEventArgs const&);
        void OnEdgeTapped(SDKTemplate::EdgeTappedListView const&, SDKTemplate::ListViewEdgeTappedEventArgs const&);

    private:
        Windows::Foundation::Collections::IObservableVector<SDKTemplate::Contact> Contacts;
        event_token backRequestedToken{};

        void UpdateSelectionUI();
        void OnBackRequested(Windows::Foundation::IInspectable const&, Windows::UI::Core::BackRequestedEventArgs const& e);
        void SetCommandsVisibility(Windows::UI::Xaml::Controls::ListView const& listView);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario4_EdgeTappedListView : Scenario4_EdgeTappedListViewT<Scenario4_EdgeTappedListView, implementation::Scenario4_EdgeTappedListView>
    {
    };
}
