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

#include "Scenario3_ShowHideMethods.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    /// <summary>
    /// Sample page to call TryShow/TryHide methods.
    /// </summary>
    struct Scenario3_ShowHideMethods : Scenario3_ShowHideMethodsT<Scenario3_ShowHideMethods>
    {
        Scenario3_ShowHideMethods();

        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        void WordListBox_OnKeyUp(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs const& e);
        void OnFadeOutCompleted(Windows::Foundation::IInspectable const& sender, Windows::Foundation::IInspectable const& e);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario3_ShowHideMethods : Scenario3_ShowHideMethodsT<Scenario3_ShowHideMethods, implementation::Scenario3_ShowHideMethods>
    {
    };
}
