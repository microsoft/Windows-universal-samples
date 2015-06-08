//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// Scenario2.xaml.cpp
// Implementation of the Scenario2 class
//

#include "pch.h"
#include "Scenario2.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Text;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario2::Scenario2()
{
    InitializeComponent();

    DisplayInformation^ displayInformation = DisplayInformation::GetForCurrentView();
    token = (displayInformation->DpiChanged += ref new TypedEventHandler<DisplayInformation^, Platform::Object^>(this, &Scenario2::DisplayProperties_DpiChanged));

    DefaultLayoutText->FontSize = PxFromPt(20);  // xaml fontsize is in pixels.
}

double Scenario2::PtFromPx(double pixel)
{
    return pixel * 72 / 96;
}

double Scenario2::PxFromPt(double pt)
{
    return pt * 96 / 72;
}

String^ Scenario2::StringFromDouble(double x)
{
    // Round to the nearest tenth for display.
    return (((int)(x * 10 + 0.5)) / 10.0).ToString();
}

void Scenario2::OutputSettings(double rawPixelsPerViewPixel, FrameworkElement^ rectangle, TextBlock^ viewPxText, TextBlock^ rawPxText, TextBlock^ fontTextBlock)
{
    // Get the size of the rectangle in view pixels and calulate the size in raw pixels.
    double sizeInViewPx = rectangle->Width;
    double sizeInRawPx = sizeInViewPx * rawPixelsPerViewPixel;

    viewPxText->Text = StringFromDouble(sizeInViewPx) + " view px";
    rawPxText->Text = StringFromDouble(sizeInRawPx) + " raw px";

    double fontSize = PtFromPx(fontTextBlock->FontSize);
    fontTextBlock->Text = StringFromDouble(fontSize) + "pt";
}

void Scenario2::ResetOutput()
{
    ResolutionTextBlock->Text = StringFromDouble(Window::Current->Bounds.Width) + "x" + StringFromDouble(Window::Current->Bounds.Height);

    DisplayInformation^ displayInformation = DisplayInformation::GetForCurrentView();
    double rawPixelsPerViewPixel = displayInformation->RawPixelsPerViewPixel;

    // Set the override rectangle size and override text font size by taking our desired
    // size in raw pixels and converting it to view pixels.
    const double rectSizeInRawPx = 100;
    const double rectSizeInViewPx = rectSizeInRawPx / rawPixelsPerViewPixel;
    OverrideLayoutRect->Width = rectSizeInViewPx;
    OverrideLayoutRect->Height = rectSizeInViewPx;

    const double fontSizeInRawPx = PxFromPt(20);
    const double fontSizeInViewPx = fontSizeInRawPx / rawPixelsPerViewPixel;
    OverrideLayoutText->FontSize = fontSizeInViewPx;

    // Output settings for controls with default scaling behavior.
    OutputSettings(rawPixelsPerViewPixel, DefaultLayoutRect, DefaultRelativePx, DefaultPhysicalPx, DefaultLayoutText);
    // Output settings for override controls.
    OutputSettings(rawPixelsPerViewPixel, OverrideLayoutRect, OverrideRelativePx, OverridePhysicalPx, OverrideLayoutText);
}

void Scenario2::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResetOutput();
}

void Scenario2::OnNavigatedFrom(NavigationEventArgs^ e)
{
    DisplayInformation^ displayInformation = DisplayInformation::GetForCurrentView();
    displayInformation->DpiChanged -= token;
}

void Scenario2::DisplayProperties_DpiChanged(DisplayInformation^ sender, Platform::Object^ args)
{
    ResetOutput();
}
