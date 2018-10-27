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

#include "Scenario1_Basic.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_Basic sealed
    {
    public:
        Scenario1_Basic();

    protected:
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ rootPage;

        // What is the program's last known full-screen state?
        // We use this to detect when the system forced us out of full-screen mode.
        bool isLastKnownFullScreen = Windows::UI::ViewManagement::ApplicationView::GetForCurrentView()->IsFullScreenMode;

        void ToggleFullScreenModeButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ShowStandardSystemOverlaysButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void UseMinimalOverlaysCheckBox_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnWindowResize(Platform::Object^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ e);
        void UpdateContent();
        void OnKeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);

        Windows::Foundation::EventRegistrationToken windowResizeToken;
        Windows::Foundation::EventRegistrationToken keyDownToken;
    };
}
