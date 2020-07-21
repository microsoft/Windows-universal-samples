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
#include "MasterDetailPage.g.h"

namespace winrt::MasterDetailApp::implementation
{
    struct MasterDetailPage : MasterDetailPageT<MasterDetailPage>
    {
        MasterDetailPage();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        // These methods are public so they can be called by binding.
        void LayoutRoot_Loaded(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void AdaptiveStates_CurrentStateChanged(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::VisualStateChangedEventArgs const& e);
        void MasterListView_ItemClick(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Controls::ItemClickEventArgs const& e);

    private:
        Windows::Foundation::Collections::IVector<MasterDetailApp::ViewModels::ItemViewModel> m_items;
        MasterDetailApp::ViewModels::ItemViewModel m_lastSelectedItem{ nullptr };

        void UpdateForVisualState(Windows::UI::Xaml::VisualState const& newState, Windows::UI::Xaml::VisualState const& oldState = nullptr);
        void EnableContentTransitions();
        void DisableContentTransitions();
    };
}

namespace winrt::MasterDetailApp::factory_implementation
{
    struct MasterDetailPage : MasterDetailPageT<MasterDetailPage, implementation::MasterDetailPage>
    {
    };
}
