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
// CalibrationBar.xaml.h
// Declaration of the CalibrationBar class
//

#pragma once

#include "pch.h"
#include "CalibrationBar.g.h"

namespace Calibration
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class CalibrationBar sealed
    {
    public:
        CalibrationBar();

        void Hide();
        void RequestCalibration(Windows::Devices::Sensors::MagnetometerAccuracy accuracy);

    private:
        // Constants specifying when the calibration bar is shown:
        //   * suppress duration determines how long (in ticks) to wait before 
        //     showing the calibration bar again 
        //   * auto dismiss time determines when to auto dismiss the bar
        //     if it's shown too long
        static const ULONGLONG SUPPRESS_DURATION_APPROXIMATE_TICKS = 6000000000; // 10 minutes
        static const ULONGLONG SUPPRESS_DURATION_UNRELIABLE_TICKS = 3000000000; // 5 minutes
        static const LONGLONG CALIBRATION_POPUP_AUTO_DISMISS_TIME_TICKS = 300000000; // 30 seconds

        ULARGE_INTEGER _lastCalibrationTime;
        bool _barDismissed;
        Windows::UI::Xaml::DispatcherTimer^ _calibrationTimer;
        Windows::Foundation::EventRegistrationToken _windowSizeEventToken;

        void InvalidateLayout();
        bool CanShowCalibrationBar(Windows::Devices::Sensors::MagnetometerAccuracy accuracy);
        void ShowGuidance();
        void HideGuidance();

        void ShowGuidanceButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void HideGuidanceButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void DismissPopup_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnAutoDismissTimerExpired(Platform::Object^ sender, Platform::Object^ e);
        void OnWindowSizeChanged(Platform::Object^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ e);
    };
}
