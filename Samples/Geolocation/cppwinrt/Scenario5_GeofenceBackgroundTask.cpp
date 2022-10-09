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
#include "Scenario5_GeofenceBackgroundTask.h"
#include "Scenario5_GeofenceBackgroundTask.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::ApplicationModel::Background;
using namespace winrt::Windows::Devices::Geolocation;
using namespace winrt::Windows::Devices::Geolocation::Geofencing;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario5_GeofenceBackgroundTask::Scenario5_GeofenceBackgroundTask()
    {
        InitializeComponent();
    }

    void Scenario5_GeofenceBackgroundTask::OnNavigatedTo(NavigationEventArgs const&)
    {
        // See if our background task is already registered.
        m_geofenceTask = LookupBackgroundTask(BackgroundTaskName);

        if (m_geofenceTask != nullptr)
        {
            // Associate an event handler with the existing background task
            m_taskCompletedToken = m_geofenceTask.Completed({ get_weak(), &Scenario5_GeofenceBackgroundTask::OnCompleted });
        }
        FillEventListBoxWithExistingEvents();
        UpdateButtonStates();
    }

    void Scenario5_GeofenceBackgroundTask::RegisterBackgroundTask(IInspectable const&, RoutedEventArgs const&)
    {
        // Register the background task without checking whether the user has enabled background execution.
        // If it's disabled, and then the user later enables background execution, the background task will be ready to run.
        // Create a new background task builder
        BackgroundTaskBuilder geofenceTaskBuilder;

        geofenceTaskBuilder.Name(BackgroundTaskName);
        geofenceTaskBuilder.TaskEntryPoint(BackgroundTaskEntryPoint);

        // Create a new location trigger
        LocationTrigger trigger(LocationTriggerType::Geofence);

        // Associate the location trigger with the background task builder
        geofenceTaskBuilder.SetTrigger(trigger);

        // Register the background task
        m_geofenceTask = geofenceTaskBuilder.Register();

        // Associate an event handler with the new background task
        m_taskCompletedToken = m_geofenceTask.Completed({ get_weak(), &Scenario5_GeofenceBackgroundTask::OnCompleted });

        UpdateButtonStates();

        CheckBackgroundAndRequestLocationAccess();
    }

    void Scenario5_GeofenceBackgroundTask::UnregisterBackgroundTask(IInspectable const&, RoutedEventArgs const&)
    {
        // Unregister the background task
        if (m_geofenceTask != nullptr)
        {
            m_geofenceTask.Unregister(true);
            m_geofenceTask.Completed(m_taskCompletedToken);
            m_geofenceTask = nullptr;
        }

        UpdateButtonStates();
        m_rootPage.NotifyUser(L"Geofence background task unregistered", NotifyType::StatusMessage);
    }

    // This event handler is called when the background task is completed.
    fire_and_forget Scenario5_GeofenceBackgroundTask::OnCompleted(IBackgroundTaskRegistration sender, BackgroundTaskCompletedEventArgs e)
    {
        auto lifetime = get_strong();

        // Update the UI with progress reported by the background task
        co_await resume_foreground(Dispatcher());

        try
        {
            // Rethrow any exception that occurred in the background task.
            e.CheckResult();

            // Update the UI with the completion status of the background task
            FillEventListBoxWithExistingEvents();
        }
        catch (...)
        {
            // The background task had an error
            m_rootPage.NotifyUser(to_message(), NotifyType::ErrorMessage);
        }
    }

    // Update the enable state of the register/unregister buttons
    // based on whether we have already registered a task.
    void Scenario5_GeofenceBackgroundTask::UpdateButtonStates()
    {
        bool registered = (m_geofenceTask != nullptr);
        RegisterBackgroundTaskButton().IsEnabled(!registered);
        UnregisterBackgroundTaskButton().IsEnabled(registered);
    }

    void Scenario5_GeofenceBackgroundTask::FillEventListBoxWithExistingEvents()
    {
        FillItemsFromSavedJson(GeofenceBackgroundEventsListBox(), L"BackgroundGeofenceEventCollection");
    }
}
