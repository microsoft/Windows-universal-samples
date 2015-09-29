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
#include "Scenario1_Orientation.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Navigation;

Scenario1_Orientation::Scenario1_Orientation()
{
    InitializeComponent();
}

void Scenario1_Orientation::OnNavigatedTo(NavigationEventArgs^ e)
{
    displayInfo = DisplayInformation::GetForCurrentView();
    orientationChangedToken = displayInfo->OrientationChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &Scenario1_Orientation::OnOrientationChanged);
    UpdateContent();
}

void Scenario1_Orientation::OnNavigatedFrom(NavigationEventArgs^ e)
{
    displayInfo->OrientationChanged -= orientationChangedToken;
}

void Scenario1_Orientation::OnOrientationChanged(DisplayInformation^ sender, Object^ e)
{
    UpdateContent();
}

String^ Scenario1_Orientation::OrientationsToString(DisplayOrientations orientations)
{
    std::wstring result = L"None";

    if ((orientations & DisplayOrientations::Landscape) == DisplayOrientations::Landscape)
    {
        result += L", Landscape";
    }
    if ((orientations & DisplayOrientations::Portrait) == DisplayOrientations::Portrait)
    {
        result += L", Portrait";
    }
    if ((orientations & DisplayOrientations::LandscapeFlipped) == DisplayOrientations::LandscapeFlipped)
    {
        result += L", LandscapeFlipped";
    }
    if ((orientations & DisplayOrientations::PortraitFlipped) == DisplayOrientations::PortraitFlipped)
    {
        result += L", PortraitFlipped";
    }
    if (result != L"None")
    {
        result = result.substr(6);
    }
    return ref new String(result.c_str());
}

void Scenario1_Orientation::UpdateContent()
{
    NativeOrientationTextBlock->Text = OrientationsToString(displayInfo->NativeOrientation);
    CurrentOrientationTextBlock->Text = OrientationsToString(displayInfo->CurrentOrientation);
    AutoRotationPreferencesTextBlock->Text = OrientationsToString(DisplayInformation::AutoRotationPreferences);
}

void Scenario1_Orientation::ApplyRotationLock()
{
    DisplayOrientations orientations = DisplayOrientations::None;

    if (PreferLandscape->IsChecked->Value)
    {
        orientations = orientations | DisplayOrientations::Landscape;
    }
    if (PreferPortrait->IsChecked->Value)
    {
        orientations = orientations | DisplayOrientations::Portrait;
    }
    if (PreferLandscapeFlipped->IsChecked->Value)
    {
        orientations = orientations | DisplayOrientations::LandscapeFlipped;
    }
    if (PreferPortraitFlipped->IsChecked->Value)
    {
        orientations = orientations | DisplayOrientations::PortraitFlipped;
    }
    DisplayInformation::AutoRotationPreferences = orientations;
    UpdateContent();
}
