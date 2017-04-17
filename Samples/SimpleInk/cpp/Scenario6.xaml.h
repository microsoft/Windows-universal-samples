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

#include "Scenario6.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario6 sealed
    {
    public:
        Scenario6();
    private:
        void UnprocessedInput_PointerEntered(Windows::UI::Input::Inking::InkUnprocessedInput^ sender, Windows::UI::Core::PointerEventArgs^ args);
        void UnprocessedInput_PointerEntered1(Windows::UI::Input::Inking::InkUnprocessedInput^ sender, Windows::UI::Core::PointerEventArgs^ args);

        void OnSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
        void SetCanvasSize();
        void ToggleLogs(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void InkToolbar_IsRulerButtonCheckedChanged(Windows::UI::Xaml::Controls::InkToolbar^ sender, Platform::Object^ args);
        void InkToolbar_EraseAllClicked(Windows::UI::Xaml::Controls::InkToolbar^ sender, Platform::Object^ args);
        void InkToolbar_InkDrawingAttributesChanged(Windows::UI::Xaml::Controls::InkToolbar^ sender, Platform::Object^ args);
        void InkToolbar_ActiveToolChanged(Windows::UI::Xaml::Controls::InkToolbar^ sender, Platform::Object^ args);
        Windows::Foundation::EventRegistrationToken rulerButtonCheckedChangedToken;
        Windows::Foundation::EventRegistrationToken eraseAllClickedToken;
        Windows::Foundation::EventRegistrationToken inkDrawingAttributesChangedToken;
        Windows::Foundation::EventRegistrationToken activeToolChangedToken;

        MainPage^ rootPage;
    };
}
