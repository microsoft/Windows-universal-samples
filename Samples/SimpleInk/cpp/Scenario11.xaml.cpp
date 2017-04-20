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
#include "Scenario11.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Input::Inking;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Scenario11::Scenario11()
{
    InitializeComponent();

    inkCanvas->InkPresenter->InputDeviceTypes = CoreInputDeviceTypes::Mouse | CoreInputDeviceTypes::Pen | CoreInputDeviceTypes::Touch;
    inkCanvas->InkPresenter->HighContrastAdjustment = InkHighContrastAdjustment::UseOriginalColors;
    ContrastingColorButton->IsChecked = true;

    accessibility = ref new AccessibilitySettings();
    ContrastingColorButton->Visibility =
        accessibility->HighContrast ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
    accessibility->HighContrastChanged +=
        ref new TypedEventHandler<AccessibilitySettings^, Object^>(this, &Scenario11::Accessibility_HighContrastChanged);
}

void Scenario11::Accessibility_HighContrastChanged(AccessibilitySettings^ sender, Object^ args)
{
    ContrastingColorButton->Visibility =
        accessibility->HighContrast ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
}

void Scenario11::ContrastingColorButtonCheckedChanged(Object^ sender, RoutedEventArgs^ e)
{
    if (ContrastingColorButton->IsChecked->Value)
    {
        inkCanvas->InkPresenter->HighContrastAdjustment = InkHighContrastAdjustment::UseSystemColorsWhenNecessary;
    }
    else
    {
        inkCanvas->InkPresenter->HighContrastAdjustment = InkHighContrastAdjustment::UseOriginalColors;
    }
}


void Scenario11::OnSizeChanged(Object^ sender, SizeChangedEventArgs^ e)
{
    HelperFunctions::UpdateCanvasSize(RootGrid, outputGrid, inkCanvas);
}

