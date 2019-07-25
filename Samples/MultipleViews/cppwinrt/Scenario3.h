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

#include "Scenario3.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario3 : Scenario3T<Scenario3>
    {
        Scenario3();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);

        fire_and_forget AnimatedSwitch_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        event_token visibilityToken;
        event_token fadeOutToken;
        Windows::UI::Xaml::Media::Animation::Storyboard fadeOutStoryboard;
        clock::time_point lastFadeOutTime = clock::time_point::min();
        handle animationCompleteEvent;

        void CompleteCurrentAnimation();
        void Fade_Completed(Windows::Foundation::IInspectable const&, Windows::Foundation::IInspectable const&);
        void StartFadeOutContents();
        void Current_VisibilityChanged(Windows::Foundation::IInspectable const& sender, Windows::UI::Core::VisibilityChangedEventArgs const& e);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario3 : Scenario3T<Scenario3, implementation::Scenario3>
    {
    };
}
