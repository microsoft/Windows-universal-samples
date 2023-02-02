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

#include "Scenario2_SupportedTactileFeedback.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario2_SupportedTactileFeedback : Scenario2_SupportedTactileFeedbackT<Scenario2_SupportedTactileFeedback>
    {
        Scenario2_SupportedTactileFeedback();

        void Pointer_Entered(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void Pointer_Exited(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e);

    private:
        Windows::Devices::Haptics::SimpleHapticsController hapticsController{ nullptr };
        Windows::Devices::Input::PenDevice penDevice{ nullptr };
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_SupportedTactileFeedback : Scenario2_SupportedTactileFeedbackT<Scenario2_SupportedTactileFeedback, implementation::Scenario2_SupportedTactileFeedback>
    {
    };
}
