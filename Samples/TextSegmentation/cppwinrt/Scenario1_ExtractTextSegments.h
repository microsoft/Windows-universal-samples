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

#include "Scenario1_ExtractTextSegments.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_ExtractTextSegments : Scenario1_ExtractTextSegmentsT<Scenario1_ExtractTextSegments>
    {
        Scenario1_ExtractTextSegments();

        void WordSegmentsButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void SelectionSegmentsButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_ExtractTextSegments : Scenario1_ExtractTextSegmentsT<Scenario1_ExtractTextSegments, implementation::Scenario1_ExtractTextSegments>
    {
    };
}
