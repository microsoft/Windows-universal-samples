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

//
// Scenario3_Calibration.xaml.h
// Declaration of the Scenario3_Calibration class
//

#pragma once

#include "pch.h"
#include "Scenario3_Calibration.g.h"
#include "MainPage.xaml.h"
#include "CalibrationBar.xaml.h"

namespace OrientationCPP
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3_Calibration sealed
    {
    public:
        Scenario3_Calibration();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        SDKTemplate::MainPage^ rootPage;
        Calibration::CalibrationBar^ calibrationBar;

        void OnHighAccuracy(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnApproximateAccuracy(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnUnreliableAccuracy(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}