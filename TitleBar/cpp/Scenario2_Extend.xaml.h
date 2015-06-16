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

#include "Scenario2_Extend.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_Extend sealed
    {
    public:
        Scenario2_Extend();

    protected:
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void ExtendView_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void EnableControls_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ToggleFullScreenModeButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnWindowSizeChanged(Platform::Object^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args);
        void UpdateFullScreenModeStatus();

    private:
        MainPage^ rootPage = MainPage::Current;

        Windows::Foundation::EventRegistrationToken windowSizeChangedToken{};
    };
}
