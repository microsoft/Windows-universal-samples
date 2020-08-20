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

#include "Scenario1_Orientation.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_Orientation : Scenario1_OrientationT<Scenario1_Orientation>
    {
        Scenario1_Orientation();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        void ApplyRotationLock_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        Windows::Graphics::Display::DisplayInformation displayInfo{ nullptr };
        event_token orientationChangedToken{};

        void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation const&, Windows::Foundation::IInspectable const&);
        hstring OrientationsToString(Windows::Graphics::Display::DisplayOrientations orientations);
        void UpdateContent();
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_Orientation : Scenario1_OrientationT<Scenario1_Orientation, implementation::Scenario1_Orientation>
    {
    };
}
