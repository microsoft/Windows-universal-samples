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

#include "SecondaryViewPage.g.h"
#include "SampleConfiguration.h"

namespace winrt::SDKTemplate::implementation
{
    struct SecondaryViewPage : SecondaryViewPageT<SecondaryViewPage>
    {
        SecondaryViewPage();
        fire_and_forget SwitchAndAnimate(int32_t fromViewId);
        void HandleProtocolLaunch(Windows::Foundation::Uri const& uri);

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        fire_and_forget GoToMain_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        fire_and_forget HideView_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void SetTitle_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void ClearTitle_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        fire_and_forget ProtocolLaunch_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        Windows::UI::Xaml::Media::Animation::Storyboard m_enterAnimation{ nullptr };
        ViewLifetimeControl m_thisViewControl{ nullptr };
        event_token m_releasedToken;

        fire_and_forget View_Released(SDKTemplate::ViewLifetimeControl const& sender, Windows::Foundation::IInspectable const&);
        fire_and_forget SetTitle(hstring newTitle);
        Windows::UI::Xaml::Media::Animation::Storyboard CreateEnterAnimation();

    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct SecondaryViewPage : SecondaryViewPageT<SecondaryViewPage, implementation::SecondaryViewPage>
    {
    };
}
