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
// Scenario2_ShakeEvents.xaml.cpp
// Implementation of the Scenario2_ShakeEvents class
//

#include "pch.h"
#include "Scenario2_ShakeEvents.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario2_ShakeEvents::Scenario2_ShakeEvents()
{
    InitializeComponent();
}

void Scenario2_ShakeEvents::OnNavigatedTo(NavigationEventArgs^ e)
{
    accelerometer = Accelerometer::GetDefault(rootPage->AccelerometerReadingType);
    if (accelerometer != nullptr)
    {
        rootPage->NotifyUser(rootPage->AccelerometerReadingType.ToString() + " accelerometer ready", NotifyType::StatusMessage);
        ScenarioEnableButton->IsEnabled = true;
    }
    else
    {
        rootPage->NotifyUser(rootPage->AccelerometerReadingType.ToString() + " accelerometer not found", NotifyType::ErrorMessage);
    }
}

void Scenario2_ShakeEvents::OnNavigatedFrom(NavigationEventArgs^ e)
{
    if (ScenarioDisableButton->IsEnabled)
    {
        ScenarioDisable();
    }
}

/// <summary>
/// This is the event handler for VisibilityChanged events. You would register for these notifications
/// if handling sensor data when the app is not visible could cause unintended actions in the app.
/// </summary>
/// <param name="sender"></param>
/// <param name="e">
/// Event data that can be examined for the current visibility state.
/// </param>
void Scenario2_ShakeEvents::VisibilityChanged(Object^ sender, VisibilityChangedEventArgs^ e)
{
    // The app should watch for VisibilityChanged events to disable and re-enable sensor input as appropriate
    if (ScenarioDisableButton->IsEnabled)
    {
        if (e->Visible)
        {
            // Re-enable sensor input
            shakenToken = accelerometer->Shaken += ref new TypedEventHandler<Accelerometer^, AccelerometerShakenEventArgs^>(this, &Scenario2_ShakeEvents::Shaken);
        }
        else
        {
            // Disable sensor input
            accelerometer->Shaken -= shakenToken;
        }
    }
}

void Scenario2_ShakeEvents::Shaken(Accelerometer^ sender, AccelerometerShakenEventArgs^ e)
{
    shakeCounter++;

    // We need to dispatch to the UI thread to display the output
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler(
            [this]()
            {
                ScenarioOutputText->Text = shakeCounter.ToString();
            },
            CallbackContext::Any
            )
        );
}

void Scenario2_ShakeEvents::ScenarioEnable()
{
    visibilityToken = Window::Current->VisibilityChanged += ref new WindowVisibilityChangedEventHandler(this, &Scenario2_ShakeEvents::VisibilityChanged);
    shakenToken = accelerometer->Shaken += ref new TypedEventHandler<Accelerometer^, AccelerometerShakenEventArgs^>(this, &Scenario2_ShakeEvents::Shaken);

    ScenarioEnableButton->IsEnabled = false;
    ScenarioDisableButton->IsEnabled = true;
}

void Scenario2_ShakeEvents::ScenarioDisable()
{
    Window::Current->VisibilityChanged -= visibilityToken;
    accelerometer->Shaken -= shakenToken;

    ScenarioEnableButton->IsEnabled = true;
    ScenarioDisableButton->IsEnabled = false;
}
