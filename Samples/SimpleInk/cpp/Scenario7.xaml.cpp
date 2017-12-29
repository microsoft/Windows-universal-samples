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
#include "Scenario7.xaml.h"

#include <WindowsNumerics.h>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input::Inking;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario7::Scenario7()
{
    InitializeComponent();

    inkCanvas->InkPresenter->InputDeviceTypes = CoreInputDeviceTypes::Mouse | CoreInputDeviceTypes::Pen;

    // Customize the ruler
    auto ruler = ref new InkPresenterRuler(inkCanvas->InkPresenter);
    ruler->BackgroundColor = Colors::PaleTurquoise;
    ruler->ForegroundColor = Colors::MidnightBlue;
    ruler->Length = 800;
    ruler->AreTickMarksVisible = false;
    ruler->IsCompassVisible = false;

    // Customize the protractor
    auto protractor = ref new InkPresenterProtractor(inkCanvas->InkPresenter);
    protractor->BackgroundColor = Colors::Bisque;
    protractor->ForegroundColor = Colors::DarkGreen;
    protractor->AccentColor = Colors::Firebrick;
    protractor->AreRaysVisible = false;
    protractor->AreTickMarksVisible = false;
    protractor->IsAngleReadoutVisible = false;
    protractor->IsCenterMarkerVisible = false;
}

void Scenario7::InkToolbar_IsStencilButtonCheckedChanged(InkToolbar^ sender, InkToolbarIsStencilButtonCheckedChangedEventArgs^ args)
{
    auto stencilButton = safe_cast<InkToolbarStencilButton^>(inkToolbar->GetMenuButton(InkToolbarMenuKind::Stencil));
    BringIntoViewButton->IsEnabled = stencilButton->IsChecked->Value;
}

void Scenario7::OnNavigatedTo(NavigationEventArgs^ e)
{
    // Make the ink canvas larger than the window, so that we can demonstrate
    // scrolling and zooming.
    inkCanvas->Width = Window::Current->Bounds.Width * 2;
    inkCanvas->Height = Window::Current->Bounds.Height * 2;
}

void Scenario7::OnBringIntoView(Object^ sender, RoutedEventArgs^ e)
{
    // Set stencil Origin to Scrollviewer Viewport origin.
    // The purpose of this behavior is to allow the user to "grab" the
    // stencil and bring it into view no matter where the scrollviewer viewport
    // happens to be.  Note that this is accomplished by a simple translation
    // that adjusts to the zoom factor.  The additional ZoomFactor term is to
    // make ensure the scale of the InkPresenterStencil is invariant to Zoom.

    float3x2 viewportTransform = make_float3x2_scale(ScrollViewer->ZoomFactor) *
        make_float3x2_translation((float)ScrollViewer->HorizontalOffset, (float)ScrollViewer->VerticalOffset) *
        make_float3x2_scale(1.0f / ScrollViewer->ZoomFactor);

    auto stencilButton = safe_cast<InkToolbarStencilButton^>(inkToolbar->GetMenuButton(InkToolbarMenuKind::Stencil));
    switch (stencilButton->SelectedStencil)
    {
    case InkToolbarStencilKind::Protractor:
        stencilButton->Protractor->Transform = viewportTransform;
        break;

    case InkToolbarStencilKind::Ruler:
        stencilButton->Ruler->Transform = viewportTransform;
        break;
    }
}
