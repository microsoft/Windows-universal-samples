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

#include "Scenario1_InkCanvasTactileFeedback.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_InkCanvasTactileFeedback : Scenario1_InkCanvasTactileFeedbackT<Scenario1_InkCanvasTactileFeedback>
    {
        Scenario1_InkCanvasTactileFeedback();

        void OnSizeChanged(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::SizeChangedEventArgs const& e);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_InkCanvasTactileFeedback : Scenario1_InkCanvasTactileFeedbackT<Scenario1_InkCanvasTactileFeedback, implementation::Scenario1_InkCanvasTactileFeedback>
    {
    };
}
