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
// Scenario4_OrientationChanged.xaml.cpp
// Implementation of the Scenario4_OrientationChanged class
//

#include "pch.h"
#include "Scenario4_OrientationChanged.xaml.h"

using namespace AccelerometerCPP;
using namespace SDKTemplate;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Platform;
using namespace Windows::Graphics::Display;

Scenario4_OrientationChanged::Scenario4_OrientationChanged() : rootPage(MainPage::Current)
{
    InitializeComponent();

    // Get two instances of the accelerometer:
    // One that returns the raw accelerometer data
    accelerometerOriginal = Accelerometer::GetDefault();
    // Other on which the 'ReadingTransform' is updated so that data returned aligns with the request transformation.
    accelerometerReadingTransform = Accelerometer::GetDefault();

    if (nullptr == accelerometerOriginal || nullptr == accelerometerReadingTransform)
    {
        rootPage->NotifyUser("No accelerometerReadingTransform found", NotifyType::ErrorMessage);
    }

    displayInformation = DisplayInformation::GetForCurrentView();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario4_OrientationChanged::OnNavigatedTo(NavigationEventArgs^ e)
{
    if (nullptr == accelerometerOriginal || nullptr == accelerometerReadingTransform)
    {
        ScenarioEnableButton->IsEnabled = false;
    }
    else
    {
        ScenarioEnableButton->IsEnabled = true;
    }
    ScenarioDisableButton->IsEnabled = false;

    // Register for orientation change
    orientationChangedToken = displayInformation->OrientationChanged += ref new Windows::Foundation::TypedEventHandler<Windows::Graphics::Display::DisplayInformation ^, Platform::Object ^>(this, &AccelerometerCPP::Scenario4_OrientationChanged::OnOrientationChanged);
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario4_OrientationChanged::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // If the navigation is external to the app do not clean up.
    // This can occur on Phone when suspending the app.
    if (NavigationMode::Forward == e->NavigationMode && nullptr == e->Uri)
    {
        return;
    }

    if (ScenarioDisableButton->IsEnabled)
    {
        Window::Current->VisibilityChanged -= visibilityToken;
        accelerometerOriginal->ReadingChanged -= readingTokenOriginal;
        accelerometerReadingTransform->ReadingChanged -= readingTokenReadingTransform;

        // Restore the default report interval to release resources while the sensor is not in use
        accelerometerOriginal->ReportInterval = 0;
        accelerometerReadingTransform->ReportInterval = 0;
    }

    displayInformation->OrientationChanged -= orientationChangedToken;
}


/// <summary>
/// Invoked on pressing 'Enable' button'.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario4_OrientationChanged::ScenarioEnable(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Establish the report interval
    accelerometerOriginal->ReportInterval = accelerometerReadingTransform->MinimumReportInterval;
    accelerometerReadingTransform->ReportInterval = accelerometerReadingTransform->MinimumReportInterval;

    // Establish the ReadingTransform to align with the current display orientation, so 
    // that the accelerometer data from 'accelerometerReadingTransform' will align with the 
    // current display orientation
    accelerometerReadingTransform->ReadingTransform = displayInformation->CurrentOrientation;

    visibilityToken = Window::Current->VisibilityChanged += ref new WindowVisibilityChangedEventHandler(this, &Scenario4_OrientationChanged::VisibilityChanged);

    // Register for ReadingChanged event
    readingTokenOriginal = accelerometerOriginal->ReadingChanged += ref new TypedEventHandler<Accelerometer^, AccelerometerReadingChangedEventArgs^>(this, &Scenario4_OrientationChanged::ReadingChangedOriginal);
    readingTokenReadingTransform = accelerometerReadingTransform->ReadingChanged += ref new TypedEventHandler<Accelerometer^, AccelerometerReadingChangedEventArgs^>(this, &Scenario4_OrientationChanged::ReadingChangedReadingTransform);

    ScenarioEnableButton->IsEnabled = false;
    ScenarioDisableButton->IsEnabled = true;
}



/// <summary>
/// Invoked on pressing 'Disable' button'.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario4_OrientationChanged::ScenarioDisable(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    Window::Current->VisibilityChanged::remove(visibilityToken);
    accelerometerOriginal->ReadingChanged::remove(readingTokenOriginal);
    accelerometerReadingTransform->ReadingChanged::remove(readingTokenReadingTransform);

    // Restore the default report interval to release resources while the sensor is not in use
    accelerometerOriginal->ReportInterval = 0;
    accelerometerReadingTransform->ReportInterval = 0;

    ScenarioEnableButton->IsEnabled = true;
    ScenarioDisableButton->IsEnabled = false;

}

/// <summary>
/// This is the event handler for VisibilityChanged events. You would register for these notifications
/// if handling sensor data when the app is not visible could cause unintended actions in the app.
/// </summary>
/// <param name="sender"></param>
/// <param name="e">
/// Event data that can be examined for the current visibility state.
/// </param>
void Scenario4_OrientationChanged::VisibilityChanged(Object^ sender, VisibilityChangedEventArgs^ e)
{
    // The app should watch for VisibilityChanged events to disable and re-enable sensor input as appropriate
    if (ScenarioDisableButton->IsEnabled)
    {
        if (e->Visible)
        {
            // Re-enable sensor input (no need to restore the desired reportInterval... it is restored for us upon app resume)
            readingTokenOriginal = accelerometerOriginal->ReadingChanged += ref new TypedEventHandler<Accelerometer^, AccelerometerReadingChangedEventArgs^>(this, &Scenario4_OrientationChanged::ReadingChangedOriginal);
            readingTokenReadingTransform = accelerometerReadingTransform->ReadingChanged += ref new TypedEventHandler<Accelerometer^, AccelerometerReadingChangedEventArgs^>(this, &Scenario4_OrientationChanged::ReadingChangedReadingTransform);
        }
        else
        {
            // Disable sensor input (no need to restore the default reportInterval... resources will be released upon app suspension)
            accelerometerOriginal->ReadingChanged::remove(readingTokenOriginal);
            accelerometerReadingTransform->ReadingChanged::remove(readingTokenReadingTransform);
        }
    }
}


/// <summary>
/// This is the event handler for ReadingChanged event of the 'accelerometerOriginal' and should 
/// notify of the accelerometer reading changes.
/// </summary>
/// <param name="sender"></param>
/// <param name="e">
/// Event data represents the accelerometer reading in its original reference frame.
/// </param>
void Scenario4_OrientationChanged::ReadingChangedOriginal(Accelerometer^ sender, AccelerometerReadingChangedEventArgs^ e)
{
    // We need to dispatch to the UI thread to display the output
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler(
        [this, e]()
        {
            AccelerometerReading^ reading = e->Reading;

            ScenarioOutput_X_Original->Text = reading->AccelerationX.ToString();
            ScenarioOutput_Y_Original->Text = reading->AccelerationY.ToString();
            ScenarioOutput_Z_Original->Text = reading->AccelerationZ.ToString();
        },
        CallbackContext::Any
        )
        );
}


/// <summary>
/// This is the event handler for ReadingChanged event of the 'accelerometerReadingTransform' and should 
/// notify of the accelerometer reading changes.
/// </summary>
/// <param name="sender"></param>
/// <param name="e">
/// Event data represents the accelerometer reading in the reference frame of the current 
// display orientation (as set to 'ReadingTransform' property of the sensor).
/// </param>
void Scenario4_OrientationChanged::ReadingChangedReadingTransform(Accelerometer^ sender, AccelerometerReadingChangedEventArgs^ e)
{
    // We need to dispatch to the UI thread to display the output
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler(
        [this, e]()
        {
            AccelerometerReading^ reading = e->Reading;

            ScenarioOutput_X_ReadingTransform->Text = reading->AccelerationX.ToString();
            ScenarioOutput_Y_ReadingTransform->Text = reading->AccelerationY.ToString();
            ScenarioOutput_Z_ReadingTransform->Text = reading->AccelerationZ.ToString();
        },
        CallbackContext::Any
        )
        );
}


/// <summary>
/// This is the event handler for OrientationChanged events. You would register for these notifications
/// if sensor data needs to be transformed to align with the current display orientation.
/// </summary>
/// <param name="sender">
/// DisplayInformation object from which the new Orientation can be determined
/// </param>
/// <param name="args"> </param>
void Scenario4_OrientationChanged::OnOrientationChanged(Windows::Graphics::Display::DisplayInformation ^sender, Platform::Object ^args)
{
    if (nullptr != accelerometerReadingTransform)
    {
        accelerometerReadingTransform->ReadingTransform = sender->CurrentOrientation;
    }
}
