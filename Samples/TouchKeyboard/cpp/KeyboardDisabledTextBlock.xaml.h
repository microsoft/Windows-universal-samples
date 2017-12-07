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

//
// KeyboardDisabledTextBlock.xaml.h
// Declaration of the KeyboardDisabledTextBlock class
//

#pragma once

#include "pch.h"
#include "KeyboardDisabledTextBlock.g.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class KeyboardDisabledTextBlock sealed
    {
    public:
        KeyboardDisabledTextBlock();

    protected:
        virtual Windows::UI::Xaml::Automation::Peers::AutomationPeer^ OnCreateAutomationPeer() override;
        virtual void OnGotFocus(Windows::UI::Xaml::RoutedEventArgs^ e) override;
        virtual void OnLostFocus(Windows::UI::Xaml::RoutedEventArgs^ e) override;
        virtual void OnTapped(Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e) override;
        virtual void OnKeyDown(Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e) override;
    };
}
