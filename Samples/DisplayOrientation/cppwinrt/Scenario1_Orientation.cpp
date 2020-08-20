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
#include "Scenario1_Orientation.h"
#include "Scenario1_Orientation.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Graphics::Display;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_Orientation::Scenario1_Orientation()
    {
        InitializeComponent();
    }

    void Scenario1_Orientation::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&)
    {
        displayInfo = DisplayInformation::GetForCurrentView();
        orientationChangedToken = displayInfo.OrientationChanged({ get_weak(), &Scenario1_Orientation::OnOrientationChanged });
        UpdateContent();
    }

    void Scenario1_Orientation::OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const&)
    {
        displayInfo.OrientationChanged(orientationChangedToken);
    }

    void Scenario1_Orientation::OnOrientationChanged(DisplayInformation const&, IInspectable const&)
    {
        UpdateContent();
    }

    hstring Scenario1_Orientation::OrientationsToString(DisplayOrientations orientations)
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
        return hstring{ result };
    }

    void Scenario1_Orientation::UpdateContent()
    {
        NativeOrientationTextBlock().Text(OrientationsToString(displayInfo.NativeOrientation()));
        CurrentOrientationTextBlock().Text(OrientationsToString(displayInfo.CurrentOrientation()));
        AutoRotationPreferencesTextBlock().Text(OrientationsToString(DisplayInformation::AutoRotationPreferences()));
    }

    void Scenario1_Orientation::ApplyRotationLock_Click(IInspectable const&, RoutedEventArgs const&)
    {
        DisplayOrientations orientations = DisplayOrientations::None;

        if (PreferLandscape().IsChecked().Value())
        {
            orientations = orientations | DisplayOrientations::Landscape;
        }
        if (PreferPortrait().IsChecked().Value())
        {
            orientations = orientations | DisplayOrientations::Portrait;
        }
        if (PreferLandscapeFlipped().IsChecked().Value())
        {
            orientations = orientations | DisplayOrientations::LandscapeFlipped;
        }
        if (PreferPortraitFlipped().IsChecked().Value())
        {
            orientations = orientations | DisplayOrientations::PortraitFlipped;
        }
        DisplayInformation::AutoRotationPreferences(orientations);
        UpdateContent();
    }
}
