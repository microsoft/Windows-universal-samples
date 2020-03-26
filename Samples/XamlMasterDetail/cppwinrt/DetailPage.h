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
#include "DetailPage.g.h"

namespace winrt::MasterDetailApp::implementation
{
    struct DetailPage : DetailPageT<DetailPage>
    {
        DetailPage();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        static void RegisterDependencyProperties();

        ViewModels::ItemViewModel Item() { return GetValue(s_itemProperty).as<ViewModels::ItemViewModel>(); }
        void Item(ViewModels::ItemViewModel value) { SetValue(s_itemProperty, value); }

        static Windows::UI::Xaml::DependencyProperty ItemProperty() { return s_itemProperty; }

        void PageRoot_Loaded(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void PageRoot_Unloaded(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void Window_SizeChanged(Windows::Foundation::IInspectable const& sender, Windows::UI::Core::WindowSizeChangedEventArgs const& e);
        void DetailPage_BackRequested(Windows::Foundation::IInspectable const& sender, Windows::UI::Core::BackRequestedEventArgs const& args);

    private:
        event_token m_sizeChangedEventRegistrationToken{};
        event_token m_backRequestedEventRegistrationToken{};

        static Windows::UI::Xaml::DependencyProperty s_itemProperty;

        void ReplaceLastBackStackEntryParameter(Windows::Foundation::IInspectable const& parameter);
        void OnBackRequested();
        void NavigateBackForWideState(bool useTransition);
        bool ShouldGoToWideState();
    };
}

namespace winrt::MasterDetailApp::factory_implementation
{
    struct DetailPage : DetailPageT<DetailPage, implementation::DetailPage>
    {
    };
}
