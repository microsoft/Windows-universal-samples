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

#include "2-PointerPointProperties.g.h"
#include "MainPage.xaml.h"
#include <map>

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2 sealed
    {
    public:
        Scenario2();
    private:
        void Pointer_Entered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
        void Pointer_Pressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
        void Pointer_Moved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
        void Pointer_Released(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
        void Pointer_Wheel_Changed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);

        void CreateOrUpdatePropertyPopUp(Windows::UI::Input::PointerPoint^ currentPoint);
        Platform::String^ GetPointerProperties(Windows::UI::Input::PointerPoint^ currentPoint);
        Platform::String^ GetDeviceSpecificProperties(Windows::UI::Input::PointerPoint^ currentPoint);
        Platform::String^ GetMouseProperties(Windows::UI::Input::PointerPoint^ currentPoint);
        Platform::String^ GetTouchProperties(Windows::UI::Input::PointerPoint^ currentPoint);
        Platform::String^ GetPenProperties(Windows::UI::Input::PointerPoint^ currentPoint);

        void RenderPropertyPopUp(Platform::String^ pointerProperties, Platform::String^ deviceSpecificProperties, Windows::UI::Input::PointerPoint^ currentPoint);
        void HidePropertyPopUp(Windows::UI::Input::PointerPoint^ currentPoint);

        MainPage^ rootPage = MainPage::Current;
        std::map<unsigned int, Windows::UI::Xaml::Controls::StackPanel^> popups;
    };
}
