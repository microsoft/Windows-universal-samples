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

#include "Scenario3_InkingFeedback.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario3_InkingFeedback : Scenario3_InkingFeedbackT<Scenario3_InkingFeedback>
    {
        Scenario3_InkingFeedback();

        void HapticCanvas_Entered(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void HapticCanvas_Exited(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e);

    private:
        Windows::Devices::Haptics::SimpleHapticsController hapticsController{ nullptr };
        Windows::Devices::Input::PenDevice penDevice{ nullptr };
        Windows::Devices::Haptics::SimpleHapticsControllerFeedback currentFeedback{ nullptr };

        Windows::Devices::Haptics::SimpleHapticsControllerFeedback GetSelectedFeedbackOrFallback(std::wstring& message);

    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario3_InkingFeedback : Scenario3_InkingFeedbackT<Scenario3_InkingFeedback, implementation::Scenario3_InkingFeedback>
    {
    };
}
