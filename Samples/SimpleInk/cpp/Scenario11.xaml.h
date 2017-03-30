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

#include "Scenario11.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario11 sealed
    {
    public:
        Scenario11();

    private:
        Windows::UI::ViewManagement::AccessibilitySettings^ accessibility;

        void Accessibility_HighContrastChanged(Windows::UI::ViewManagement::AccessibilitySettings^ sender, Platform::Object^ args);
        void ContrastingColorButtonCheckedChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
    };
}
