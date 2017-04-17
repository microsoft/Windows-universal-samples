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
// Scenario3_BackgroundTask.xaml.cpp
// Implementation of the Scenario3 class
//

#include "pch.h"
#include "Scenario3_BackgroundTask.xaml.h"

using namespace SDKTemplate;
using namespace SDKTemplate::GeolocationCPP;

using namespace concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::Devices::Geolocation;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario3::Scenario3() : rootPage(MainPage::Current), sampleBackgroundTaskName("SampleLocationBackgroundTask"), sampleBackgroundTaskEntryPoint("BackgroundTask.LocationBackgroundTask")
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario3::OnNavigatedTo(NavigationEventArgs^ e)
{
    // Look for an already registered background task
    geolocTask = nullptr;

    auto iter = BackgroundTaskRegistration::AllTasks->First();
    while (iter->HasCurrent)
    {
        auto task = iter->Current;
        if (task->Value->Name == sampleBackgroundTaskName)
        {
            geolocTask = safe_cast<BackgroundTaskRegistration^>(task->Value);
            break;
        }
        iter->MoveNext();
    }

    if (geolocTask != nullptr)
    {
        // Register for background task completion notifications
        taskCompletedToken = geolocTask->Completed::add(ref new BackgroundTaskCompletedEventHandler(this, &Scenario3::OnCompleted));

        try
        {
            // Check the background access status of the application and display the appropriate status message
            switch (BackgroundExecutionManager::GetAccessStatus())
            {
            case BackgroundAccessStatus::AlwaysAllowed:
            case BackgroundAccessStatus::AllowedSubjectToSystemPolicy:
                rootPage->NotifyUser("Background task is already registered. Waiting for next update...", NotifyType::StatusMessage);
                break;

            default:
                rootPage->NotifyUser("Background tasks may be disabled for this app", NotifyType::ErrorMessage);
                break;
            }
        }
        catch (Exception^ ex)
        {
            rootPage->NotifyUser(ex->ToString(), NotifyType::ErrorMessage);
        }

        UpdateButtonStates(/*registered:*/ true);
    }
    else
    {
        UpdateButtonStates(/*registered:*/ false);
    }
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario3::OnNavigatedFrom(NavigationEventArgs^ e)
{
    if (geolocTask != nullptr)
    {
        // remove the event handler
        geolocTask->Completed::remove(taskCompletedToken);
    }
}

void Scenario3::RegisterBackgroundTask(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    try
    {
        // Get permission for a background task from the user. If the user has already answered once,
        // this does nothing and the user must manually update their preference via PC Settings.
        task<BackgroundAccessStatus> requestAccessTask(BackgroundExecutionManager::RequestAccessAsync());
        requestAccessTask.then([this](BackgroundAccessStatus backgroundAccessStatus)
        {
            // Create a new background task builder
            BackgroundTaskBuilder^ geolocTaskBuilder = ref new BackgroundTaskBuilder();

            geolocTaskBuilder->Name = sampleBackgroundTaskName;
            geolocTaskBuilder->TaskEntryPoint = sampleBackgroundTaskEntryPoint;

            // Create a new timer triggering at a 15 minute interval
            auto trigger = ref new TimeTrigger(15, false);

            // Associate the timer trigger with the background task builder
            geolocTaskBuilder->SetTrigger(trigger);

            // Register the background task
            geolocTask = geolocTaskBuilder->Register();

            // Register for background task completion notifications
            taskCompletedToken = geolocTask->Completed::add(ref new BackgroundTaskCompletedEventHandler(this, &Scenario3::OnCompleted));

            UpdateButtonStates(/*registered:*/ true);

            // Check the background access status of the application and display the appropriate status message
            switch (backgroundAccessStatus)
            {
            case BackgroundAccessStatus::AlwaysAllowed:
            case BackgroundAccessStatus::AllowedSubjectToSystemPolicy:
                rootPage->NotifyUser("Background task registered.", NotifyType::StatusMessage);

                // Need to request access to location
                // This must be done with background task registration
                // because the background task cannot display UI
                RequestLocationAccess();
                break;

            default:
                rootPage->NotifyUser("Background tasks may be disabled for this app", NotifyType::ErrorMessage);
                break;

            }
        });
    }
    catch (Exception^ ex)
    {
        rootPage->NotifyUser(ex->ToString(), NotifyType::ErrorMessage);
        UpdateButtonStates(/*registered:*/ false);
    }
}

void Scenario3::UnregisterBackgroundTask(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Unregister the background task
    if (geolocTask != nullptr)
    {
        geolocTask->Unregister(true);
        geolocTask = nullptr;
    }

    ScenarioOutput_Latitude->Text = "No data";
    ScenarioOutput_Longitude->Text = "No data";
    ScenarioOutput_Accuracy->Text = "No data";
    UpdateButtonStates(/*registered:*/ false);
    rootPage->NotifyUser("Background task unregistered.", NotifyType::StatusMessage);
}

void Scenario3::OnCompleted(BackgroundTaskRegistration^ task, Windows::ApplicationModel::Background::BackgroundTaskCompletedEventArgs^ args)
{
    // Update the UI with progress reported by the background task
    // We need to dispatch to the UI thread to display the output
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler(
        [this, args]()
    {
        try
        {
            // Throw an exception if the background task had an unrecoverable error
            args->CheckResult();

            // Update the UI with the completion status of the background task
            auto settings = ApplicationData::Current->LocalSettings->Values;
            if (settings->HasKey("Status"))
            {
                rootPage->NotifyUser(safe_cast<String^>(settings->Lookup("Status")), NotifyType::StatusMessage);
            }

            // Extract and display Latitude
            if (settings->HasKey("Latitude"))
            {
                ScenarioOutput_Latitude->Text = safe_cast<String^>(settings->Lookup("Latitude"));
            }
            else
            {
                ScenarioOutput_Latitude->Text = "No data";
            }

            // Extract and display Longitude
            if (settings->HasKey("Longitude"))
            {
                ScenarioOutput_Longitude->Text = safe_cast<String^>(settings->Lookup("Longitude"));
            }
            else
            {
                ScenarioOutput_Longitude->Text = "No data";
            }

            // Extract and display Accuracy
            if (settings->HasKey("Accuracy"))
            {
                ScenarioOutput_Accuracy->Text = safe_cast<String^>(settings->Lookup("Accuracy"));
            }
            else
            {
                ScenarioOutput_Accuracy->Text = "No data";
            }
        }
        catch (Exception^ ex)
        {
            // The background task had an error
            rootPage->NotifyUser(ex->Message, NotifyType::ErrorMessage);
        }
    },
        CallbackContext::Any
        )
        );
}

/// <summary>
/// Update the enable state of the register/unregister buttons.
/// </summary>
void Scenario3::UpdateButtonStates(bool registered)
{
    auto uiDelegate = [this, registered]()
    {
        RegisterBackgroundTaskButton->IsEnabled = !registered;
        UnregisterBackgroundTaskButton->IsEnabled = registered;
    };
    auto handler = ref new Windows::UI::Core::DispatchedHandler(uiDelegate, Platform::CallbackContext::Any);

    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, handler);
}

void Scenario3::RequestLocationAccess()
{
    task<GeolocationAccessStatus> geolocationAccessRequestTask(Windows::Devices::Geolocation::Geolocator::RequestAccessAsync());
    geolocationAccessRequestTask.then([this](task<GeolocationAccessStatus> accessStatusTask)
    {
        auto accessStatus = accessStatusTask.get();

        switch (accessStatus)
        {
        case GeolocationAccessStatus::Allowed:
            break;
        case GeolocationAccessStatus::Denied:
            rootPage->NotifyUser("Access to location is denied.", NotifyType::ErrorMessage);
            break;
        case GeolocationAccessStatus::Unspecified:
            rootPage->NotifyUser("Unspecified error!", NotifyType::ErrorMessage);
            break;
        }
    });
}