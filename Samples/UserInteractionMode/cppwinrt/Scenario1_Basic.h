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
#include "SampleConfiguration.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_Basic : Scenario1_BasicT<Scenario1_Basic>
    {
        Scenario1_Basic();

        Windows::UI::Xaml::Style CheckBoxStyle()
        {
            return m_checkBoxStyle;
        }

        Windows::UI::ViewManagement::UserInteractionMode InteractionMode()
        {
            return m_interactionMode;
        }

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(event_token const& token) noexcept;

    private:
        SDKTemplate::MainPage rootPage{ SDKTemplate::implementation::MainPage::Current() };
        event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
        Windows::UI::Xaml::Style m_checkBoxStyle{ nullptr };
        Windows::UI::ViewManagement::UserInteractionMode m_interactionMode;
        event_token windowResizeToken;

    private:
        void OnWindowResize(Windows::Foundation::IInspectable const&, Windows::UI::Core::WindowSizeChangedEventArgs const& e);
        void UpdateContent();

    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_Basic : Scenario1_BasicT<Scenario1_Basic, implementation::Scenario1_Basic>
    {
    };
}
