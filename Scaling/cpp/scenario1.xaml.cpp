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
// Scenario1.xaml.cpp
// Implementation of the Scenario1 class
//

#include "pch.h"
#include "Scenario1.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;

Scenario1::Scenario1()
{
    InitializeComponent();
    DisplayInformation^ displayInformation = DisplayInformation::GetForCurrentView();
    token = (displayInformation->DpiChanged += ref new TypedEventHandler<DisplayInformation^, Platform::Object^>(this, &Scenario1::DisplayProperties_DpiChanged));
}

void Scenario1::ResetOutput()
{
    DisplayInformation^ displayInformation = DisplayInformation::GetForCurrentView();
    String^ scaleValue = static_cast<int>(displayInformation->RawPixelsPerViewPixel * 100.0f + 0.5f).ToString();
    ManualLoadURL->Text = "http://www.contoso.com/imageScale" + scaleValue + ".png";
    ScalingText->Text = scaleValue + "%";
    LogicalDPIText->Text = displayInformation->LogicalDpi.ToString() + " DPI";
}

void Scenario1::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResetOutput();
}

void Scenario1::OnNavigatedFrom(NavigationEventArgs^ e)
{
    DisplayInformation^ displayInformation = DisplayInformation::GetForCurrentView();
    displayInformation->DpiChanged -= token;
}

void Scenario1::DisplayProperties_DpiChanged(DisplayInformation^ sender, Platform::Object^ args)
{
    ResetOutput();
}
