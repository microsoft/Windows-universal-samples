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

using namespace AccelerometerCPP;
using namespace SDKTemplate;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Platform;

Scenario2_ShakeEvents::Scenario2_ShakeEvents() : rootPage(MainPage::Current), shakeCounter(0)
{
    InitializeComponent();

    accelerometer = Accelerometer::GetDefault();
    if (accelerometer == nullptr)
    {
        rootPage->NotifyUser("No accelerometer found", NotifyType::ErrorMessage);
    }
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario2_ShakeEvents::OnNavigatedTo(NavigationEventArgs^ e)
{
    ScenarioEnableButton->IsEnabled = true;
    ScenarioDisableButton->IsEnabled = false;
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario2_ShakeEvents::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // If the navigation is external to the app do not clean up.
    // This can occur on Phone when suspending the app.
    if (e->NavigationMode == NavigationMode::Forward && e->Uri == nullptr)
    {
        return;
    }

    if (ScenarioDisableButton->IsEnabled)
    {
        Window::Current->VisibilityChanged::remove(visibilityToken);
        accelerometer->Shaken::remove(shakenToken);
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
            shakenToken = accelerometer->Shaken::add(ref new TypedEventHandler<Accelerometer^, AccelerometerShakenEventArgs^>(this, &Scenario2_ShakeEvents::Shaken));
        }
        else
        {
            // Disable sensor input
            accelerometer->Shaken::remove(shakenToken);
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

void Scenario2_ShakeEvents::ScenarioEnable(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (accelerometer != nullptr)
    {
        visibilityToken = Window::Current->VisibilityChanged::add(ref new WindowVisibilityChangedEventHandler(this, &Scenario2_ShakeEvents::VisibilityChanged));
        shakenToken = accelerometer->Shaken::add(ref new TypedEventHandler<Accelerometer^, AccelerometerShakenEventArgs^>(this, &Scenario2_ShakeEvents::Shaken));

        ScenarioEnableButton->IsEnabled = false;
        ScenarioDisableButton->IsEnabled = true;
    }
    else
    {
        rootPage->NotifyUser("No accelerometer found", NotifyType::ErrorMessage);
    }
}

void Scenario2_ShakeEvents::ScenarioDisable(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    Window::Current->VisibilityChanged::remove(visibilityToken);
    accelerometer->Shaken::remove(shakenToken);

    ScenarioEnableButton->IsEnabled = true;
    ScenarioDisableButton->IsEnabled = false;
}
