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

#include "pch.h"
#include "Scenario1_Colors.h"
#include "Scenario1_Colors.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Data;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_Colors::Scenario1_Colors()
    {
        InitializeComponent();

        titleBar = ApplicationView::GetForCurrentView().TitleBar();

        // Infer the radio button selection from the title bar colors.
        if (titleBar.ButtonBackgroundColor() == nullptr)
        {
            UseStandardColors().IsChecked(true);
        }
        else if (titleBar.ButtonBackgroundColor().Value().A > 0)
        {
            UseCustomColors().IsChecked(true);
        }
        else
        {
            TransparentWhenExtended().IsChecked(true);
        }

    }

    void Scenario1_Colors::UseCustomColors_Click(IInspectable const&, RoutedEventArgs const&)
    {
        UpdateColors();
    }

    void Scenario1_Colors::UpdateColors()
    {
        if (UseStandardColors().IsChecked().Value())
        {
            // Setting colors to null returns them to system defaults.
            titleBar.BackgroundColor(nullptr);
            titleBar.ForegroundColor(nullptr);
            titleBar.InactiveBackgroundColor(nullptr);
            titleBar.InactiveForegroundColor(nullptr);

            titleBar.ButtonBackgroundColor(nullptr);
            titleBar.ButtonHoverBackgroundColor(nullptr);
            titleBar.ButtonPressedBackgroundColor(nullptr);
            titleBar.ButtonInactiveBackgroundColor(nullptr);

            titleBar.ButtonForegroundColor(nullptr);
            titleBar.ButtonHoverForegroundColor(nullptr);
            titleBar.ButtonPressedForegroundColor(nullptr);
            titleBar.ButtonInactiveForegroundColor(nullptr);
        }
        else
        {
            // Title bar colors. Alpha must be 255.
            titleBar.BackgroundColor(ColorHelper::FromArgb(255, 54, 60, 116));
            titleBar.ForegroundColor(ColorHelper::FromArgb(255, 232, 211, 162));
            titleBar.InactiveBackgroundColor(ColorHelper::FromArgb(255, 135, 141, 199));
            titleBar.InactiveForegroundColor(ColorHelper::FromArgb(255, 232, 211, 162));

            // Title bar button background colors. Alpha is respected when the view is extended
            // into the title bar (see scenario 2). Otherwise, Alpha is ignored and treated as if it were 255.
            uint8_t buttonAlpha = (uint8_t)(TransparentWhenExtended().IsChecked().Value() ? 0 : 255);
            titleBar.ButtonBackgroundColor(ColorHelper::FromArgb(buttonAlpha, 54, 60, 116));
            titleBar.ButtonHoverBackgroundColor(ColorHelper::FromArgb(buttonAlpha, 19, 21, 40));
            titleBar.ButtonPressedBackgroundColor(ColorHelper::FromArgb(buttonAlpha, 232, 211, 162));
            titleBar.ButtonInactiveBackgroundColor(ColorHelper::FromArgb(buttonAlpha, 135, 141, 199));

            // Title bar button foreground colors. Alpha must be 255.
            titleBar.ButtonForegroundColor(ColorHelper::FromArgb(255, 232, 211, 162));
            titleBar.ButtonHoverForegroundColor(ColorHelper::FromArgb(255, 255, 255, 255));
            titleBar.ButtonPressedForegroundColor(ColorHelper::FromArgb(255, 54, 60, 116));
            titleBar.ButtonInactiveForegroundColor(ColorHelper::FromArgb(255, 232, 211, 162));
        }

        m_propertyChanged(*this, PropertyChangedEventArgs(L"TitleBar"));
    }

    event_token Scenario1_Colors::PropertyChanged(PropertyChangedEventHandler const& handler)
    {
        return m_propertyChanged.add(handler);
    }

    void Scenario1_Colors::PropertyChanged(event_token const& token) noexcept
    {
        m_propertyChanged.remove(token);
    }
}

hstring winrt::to_hstring(Windows::Foundation::IReference<Windows::UI::Color> const& value)
{
    if (value == nullptr)
    {
        return L"null";
    }

    Color color = value.Value();

    return L"A: " + to_hstring(color.A) + L",  R: " + to_hstring(color.R) + L", G: " + to_hstring(color.G) + L", B: " + to_hstring(color.B);
}
