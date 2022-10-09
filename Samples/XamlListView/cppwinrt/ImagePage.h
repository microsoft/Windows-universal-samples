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
#include "ImagePage.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct ImagePage : ImagePageT<ImagePage>
    {
        ImagePage();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

    private:
        event_token backRequestedToken{};

        void OnBackRequested(Windows::Foundation::IInspectable const&, Windows::UI::Core::BackRequestedEventArgs const& e);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct ImagePage : ImagePageT<ImagePage, implementation::ImagePage>
    {
    };
}
