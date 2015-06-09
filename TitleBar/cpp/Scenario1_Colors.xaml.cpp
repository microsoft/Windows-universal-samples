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
#include "Scenario1_Colors.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario1_Colors::Scenario1_Colors() : rootPage(MainPage::Current)
{
    InitializeComponent();

    titleBar = ApplicationView::GetForCurrentView()->TitleBar;

    // Infer the radio button selection from the title bar colors.
    if (titleBar->ButtonBackgroundColor == nullptr)
    {
        UseStandardColors->IsChecked = true;
    }
    else if (titleBar->ButtonBackgroundColor->Value.A > 0)
    {
        UseCustomColors->IsChecked = true;
    }
    else
    {
        TransparentWhenExtended->IsChecked = true;
    }
    ReportColors();
}


static Platform::String^ ConvertToString(Platform::IBox<Windows::UI::Color>^ value)
{
    if (value == nullptr)
    {
        return "null";
    }

    Windows::UI::Color color = value->Value;

    return "A: " + color.A.ToString() + ",  R: " + color.R.ToString() + ", G: " + color.G.ToString() + ", B: " + color.B.ToString();
}

void Scenario1_Colors::UpdateColors()
{
    if (UseStandardColors->IsChecked->Value)
    {
        // Setting colors to nullptr returns them to system defaults.
        titleBar->BackgroundColor = nullptr;
        titleBar->ForegroundColor = nullptr;
        titleBar->InactiveBackgroundColor = nullptr;
        titleBar->InactiveForegroundColor = nullptr;

        titleBar->ButtonBackgroundColor = nullptr;
        titleBar->ButtonHoverBackgroundColor = nullptr;
        titleBar->ButtonPressedBackgroundColor = nullptr;
        titleBar->ButtonInactiveBackgroundColor = nullptr;

        titleBar->ButtonForegroundColor = nullptr;
        titleBar->ButtonHoverForegroundColor = nullptr;
        titleBar->ButtonPressedForegroundColor = nullptr;
        titleBar->ButtonInactiveForegroundColor = nullptr;
    }
    else
    {
        // Title bar colors. Alpha must be 255.
        titleBar->BackgroundColor = ColorHelper::FromArgb(255, 54, 60, 116);
        titleBar->ForegroundColor = ColorHelper::FromArgb(255, 232, 211, 162);
        titleBar->InactiveBackgroundColor = ColorHelper::FromArgb(255, 135, 141, 199);
        titleBar->InactiveForegroundColor = ColorHelper::FromArgb(255, 232, 211, 162);

        // Title bar button background colors. Alpha is respected when the view is extended
        // into the title bar (see scenario 2). Otherwise, Alpha is ignored and treated as if it were 255.
        byte buttonAlpha = (byte)(TransparentWhenExtended->IsChecked->Value ? 0 : 255);
        titleBar->ButtonBackgroundColor = ColorHelper::FromArgb(buttonAlpha, 54, 60, 116);
        titleBar->ButtonHoverBackgroundColor = ColorHelper::FromArgb(buttonAlpha, 19, 21, 40);
        titleBar->ButtonPressedBackgroundColor = ColorHelper::FromArgb(buttonAlpha, 232, 211, 162);
        titleBar->ButtonInactiveBackgroundColor = ColorHelper::FromArgb(buttonAlpha, 135, 141, 199);

        // Title bar button foreground colors. Alpha must be 255.
        titleBar->ButtonForegroundColor = ColorHelper::FromArgb(255, 232, 211, 162);
        titleBar->ButtonHoverForegroundColor = ColorHelper::FromArgb(255, 255, 255, 255);
        titleBar->ButtonPressedForegroundColor = ColorHelper::FromArgb(255, 54, 60, 116);
        titleBar->ButtonInactiveForegroundColor = ColorHelper::FromArgb(255, 232, 211, 162);
    }

    ReportColors();
}

void Scenario1_Colors::ReportColors()
{
    BackgroundColorText->Text = ConvertToString(titleBar->BackgroundColor);
    ForegroundColorText->Text = ConvertToString(titleBar->ForegroundColor);
    InactiveBackgroundColorText->Text = ConvertToString(titleBar->InactiveBackgroundColor);
    InactiveForegroundColorText->Text = ConvertToString(titleBar->InactiveForegroundColor);

    ButtonBackgroundColorText->Text = ConvertToString(titleBar->ButtonBackgroundColor);
    ButtonHoverBackgroundColorText->Text = ConvertToString(titleBar->ButtonHoverBackgroundColor);
    ButtonPressedBackgroundColorText->Text = ConvertToString(titleBar->ButtonPressedBackgroundColor);
    ButtonInactiveBackgroundColorText->Text = ConvertToString(titleBar->ButtonInactiveBackgroundColor);

    ButtonForegroundColorText->Text = ConvertToString(titleBar->ButtonForegroundColor);
    ButtonHoverForegroundColorText->Text = ConvertToString(titleBar->ButtonHoverForegroundColor);
    ButtonPressedForegroundColorText->Text = ConvertToString(titleBar->ButtonPressedForegroundColor);
    ButtonInactiveForegroundColorText->Text = ConvertToString(titleBar->ButtonInactiveForegroundColor);
}
