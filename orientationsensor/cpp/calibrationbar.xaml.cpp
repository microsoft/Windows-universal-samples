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
// CalibrationBar.xaml.cpp
// Implementation of the CalibrationBar class
//

#include "pch.h"
#include "CalibrationBar.xaml.h"

using namespace Calibration;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Platform;

static const double LAYOUT_WIDTH_1440 = 1440.0;
static const double LAYOUT_WIDTH_1080 = 1080.0;
static const double LAYOUT_WIDTH_768 = 768.0;
static const double LAYOUT_WIDTH_720 = 720.0;
static const double LAYOUT_WIDTH_540 = 540.0;

CalibrationBar::CalibrationBar() : _barDismissed(false) 
{
    InitializeComponent();

    _lastCalibrationTime.QuadPart = 0;

    TimeSpan span;
    span.Duration = CALIBRATION_POPUP_AUTO_DISMISS_TIME_TICKS;
    _calibrationTimer = ref new DispatcherTimer();
    _calibrationTimer->Tick += ref new Windows::Foundation::EventHandler<Object^>(this, &CalibrationBar::OnAutoDismissTimerExpired);
    _calibrationTimer->Interval = span;
}

/// <summary>
/// Called when the application would like to show the calibration bar to the user 
/// to improve sensor accuracy.
/// </summary>
/// <param name="accuracy">Current accuracy of the sensor.</param>
void CalibrationBar::RequestCalibration(MagnetometerAccuracy accuracy)
{
    // Only show the calibration bar if it is hidden and sufficient
    // time has passed since it was last hidden.
    if (!CalibrationPopup->IsOpen && CanShowCalibrationBar(accuracy))
    {
        InvalidateLayout();
        _windowSizeEventToken = Window::Current->SizeChanged += ref new WindowSizeChangedEventHandler(this, &CalibrationBar::OnWindowSizeChanged);
        HideGuidance(); // Calibration image hidden by default
        CalibrationPopup->IsOpen = true;
        _calibrationTimer->Start();
    }
}

/// <summary>
/// Called when sensor accuracy is sufficient and the application would like to 
/// hide the calibration bar.
/// </summary>
void CalibrationBar::Hide()
{
    if (CalibrationPopup->IsOpen)
    {
        // Hide the calibration bar and save the current time.
        FILETIME filetime;
        GetSystemTimeAsFileTime(&filetime);
        _lastCalibrationTime.HighPart = filetime.dwHighDateTime;
        _lastCalibrationTime.LowPart = filetime.dwLowDateTime;
        HideGuidance();
        CalibrationPopup->IsOpen = false;
        Window::Current->SizeChanged -= _windowSizeEventToken;
        _calibrationTimer->Stop();
    }
}

/// <summary>
/// Invoked when the window size is updated.
/// </summary>
/// <param name="sender">Instance that triggered the event.</param>
/// <param name="e">Event data describing the conditions that led to the event.</param>
void CalibrationBar::OnWindowSizeChanged(Object^ sender, WindowSizeChangedEventArgs^ e)
{
    InvalidateLayout();
}

/// <summary>
/// Updates the calibration bar layout based on the current window width.
/// </summary>
void CalibrationBar::InvalidateLayout()
{
    CalibrationGrid->Width = Window::Current->Bounds.Width;

#if (WINAPI_FAMILY == WINAPI_FAMILY_PC_APP)
    if (CalibrationGrid->Width < LAYOUT_WIDTH_768)
    {
        Grid::SetRow(DismissPopup, 1);
        Grid::SetColumn(DismissPopup, 1);

        CalibrationGridTop->Margin = Thickness(0, 24, 20, 15);
        DismissPopup->Margin = Thickness(66, 15, 0, 0);
    }
    else
    {
        Grid::SetRow(DismissPopup, 0);
        Grid::SetColumn(DismissPopup, 2);

        CalibrationGridTop->Margin = Thickness(0, 19, 66, 19);
        DismissPopup->Margin = Thickness(66, 0, 0, 0);
    }
#else
    if (CalibrationGrid->Width < LAYOUT_WIDTH_540)
    {
        Guidance->Source = ref new BitmapImage(ref new Uri("ms-appx:///Assets/mobius480.png"));
    }
    else if (CalibrationGrid->Width < LAYOUT_WIDTH_720)
    {
        Guidance->Source = ref new BitmapImage(ref new Uri("ms-appx:///Assets/mobius540.png"));
    }
    else if (CalibrationGrid->Width < LAYOUT_WIDTH_768)
    {
        Guidance->Source = ref new BitmapImage(ref new Uri("ms-appx:///Assets/mobius720.png"));
    }
    else if (CalibrationGrid->Width < LAYOUT_WIDTH_1080)
    {
        Guidance->Source = ref new BitmapImage(ref new Uri("ms-appx:///Assets/mobius768.png"));
    }
    else if (CalibrationGrid->Width < LAYOUT_WIDTH_1440)
    {
        Guidance->Source = ref new BitmapImage(ref new Uri("ms-appx:///Assets/mobius1080.png"));
    }
    else
    {
        Guidance->Source = ref new BitmapImage(ref new Uri("ms-appx:///Assets/mobius1440.png"));
    }
#endif
}

/// <summary>
/// Called to determine if sufficient time has passed since the calibration bar
/// was hidden to show the calibration bar again. This suppression time is
/// dependent on the current sensor accuracy.
/// </summary>
/// <param name="accuracy">Current accuracy of the sensor.</param>
bool CalibrationBar::CanShowCalibrationBar(MagnetometerAccuracy accuracy)
{
    bool showBar = false;
    if (!_barDismissed)
    {
        FILETIME filetime;
        ULARGE_INTEGER now;
        GetSystemTimeAsFileTime(&filetime);
        now.HighPart = filetime.dwHighDateTime;
        now.LowPart = filetime.dwLowDateTime;

        if ((accuracy == MagnetometerAccuracy::Approximate) &&
            (_lastCalibrationTime.QuadPart + SUPPRESS_DURATION_APPROXIMATE_TICKS <= now.QuadPart))
        {
            showBar = true;
        }
        else if ((accuracy == MagnetometerAccuracy::Unreliable) &&
            (_lastCalibrationTime.QuadPart + SUPPRESS_DURATION_UNRELIABLE_TICKS <= now.QuadPart))
        {
            showBar = true;
        }
    }
    return showBar;
}

/// <summary>
/// This is the click handler for the show guidance button.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void CalibrationBar::ShowGuidanceButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (Guidance->Visibility == Windows::UI::Xaml::Visibility::Collapsed)
    {
        ShowGuidance();
    }
}

/// <summary>
/// This is the click handler for the hide guidance button.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void CalibrationBar::HideGuidanceButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (Guidance->Visibility == Windows::UI::Xaml::Visibility::Visible)
    {
        HideGuidance();
    }
}

/// <summary>
/// This is the click handler for the dismiss popup button. It hides the 
/// calibration bar and prevents it from being shown again for the lifetime
/// of the app.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void CalibrationBar::DismissPopup_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Don't show the calibration bar again.
    _barDismissed = true;
    Hide();
}

/// <summary>
/// Expands the calibration bar to show the calibration image to the user.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void CalibrationBar::ShowGuidance()
{
    ShowGuidanceButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    HideGuidanceButton->Visibility = Windows::UI::Xaml::Visibility::Visible;
    Guidance->Visibility = Windows::UI::Xaml::Visibility::Visible;
}

/// <summary>
/// Collapses the calibration bar to hide the calibration image from the user.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void CalibrationBar::HideGuidance()
{
    HideGuidanceButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    ShowGuidanceButton->Visibility = Windows::UI::Xaml::Visibility::Visible;
    Guidance->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}

/// <summary>
/// This is the handler for the auto dismiss timer. After the calibraiton bar
/// has been shown for the maximum time, it is automatically hidden.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void CalibrationBar::OnAutoDismissTimerExpired(Object^ sender, Object^ e)
{
    Hide();
}