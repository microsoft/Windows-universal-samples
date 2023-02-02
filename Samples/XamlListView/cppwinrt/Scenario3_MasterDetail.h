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
#include "Scenario3_MasterDetail.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario3_MasterDetail : Scenario3_MasterDetailT<Scenario3_MasterDetail>
    {
        Scenario3_MasterDetail();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnLoaded(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void OnCurrentStateChanged(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::VisualStateChangedEventArgs const&);
        void OnSelectionChanged(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Controls::SelectionChangedEventArgs const&);
        void OnItemClick(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Controls::ItemClickEventArgs const& e);
        void AddItem(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void DeleteItem(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void DeleteItems(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void SelectItems(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void CancelSelection(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        Windows::Foundation::Collections::IObservableVector<SDKTemplate::Contact> Contacts;

        void EnableContentTransitions();
        void SelectSomethingIfPossible();
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario3_MasterDetail : Scenario3_MasterDetailT<Scenario3_MasterDetail, implementation::Scenario3_MasterDetail>
    {
    };
}
