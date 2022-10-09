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
#include "DetailsPage.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct DetailsPage : DetailsPageT<DetailsPage>
    {
        DetailsPage();

        auto SelectedContact() { return selectedContact; }

        void OnLoaded(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnUnloaded(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void DeleteItem(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        SDKTemplate::Contact selectedContact{ nullptr };
        event_token backRequestedToken{};
        event_token windowSizeChangedToken{};

        void OnBackRequested(Windows::Foundation::IInspectable const&, Windows::UI::Core::BackRequestedEventArgs const& e);
        void NavigateBackForWideState(bool useTransition);
        bool ShouldGoToWideState();
        void OnWindowSizeChanged(Windows::Foundation::IInspectable const&, Windows::UI::Core::WindowSizeChangedEventArgs const&);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct DetailsPage : DetailsPageT<DetailsPage, implementation::DetailsPage>
    {
    };
}
