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

#include "Scenario4_InteractionFeedback.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario4_InteractionFeedback : Scenario4_InteractionFeedbackT<Scenario4_InteractionFeedback>
    {
        Scenario4_InteractionFeedback();

        void MainGrid_Entered(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void MainGrid_Exited(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void SendFeedback_Clicked(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        Windows::Devices::Haptics::SimpleHapticsController hapticsController{ nullptr };
        Windows::Devices::Input::PenDevice penDevice{ nullptr };
        Windows::Devices::Haptics::SimpleHapticsControllerFeedback currentFeedback{ nullptr };

        Windows::Devices::Haptics::SimpleHapticsControllerFeedback GetSelectedFeedbackOrFallback(std::wstring& message);

    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario4_InteractionFeedback : Scenario4_InteractionFeedbackT<Scenario4_InteractionFeedback, implementation::Scenario4_InteractionFeedback>
    {
    };
}
