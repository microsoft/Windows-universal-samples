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
#include "Scenario3_BackgroundTask.h"
#include "Scenario3_BackgroundTask.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::ApplicationModel::Background;
using namespace winrt::Windows::Devices::Geolocation;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario3_BackgroundTask::Scenario3_BackgroundTask()
    {
        InitializeComponent();
    }

    void Scenario3_BackgroundTask::OnNavigatedTo(NavigationEventArgs const&)
    {
       // See if our background task is already registered.
        m_geolocTask = LookupBackgroundTask(BackgroundTaskName);

        if (m_geolocTask != nullptr)
        {
            // Associate an event handler with the existing background task
            m_taskCompletedToken = m_geolocTask.Completed({ get_weak(), &Scenario3_BackgroundTask::OnCompleted });
        }
        UpdateButtonStates();
    }

    void Scenario3_BackgroundTask::OnNavigatedFrom(NavigationEventArgs const&)
    {
        if (m_geolocTask != nullptr)
        {
            // Remove the event handler
            m_geolocTask.Completed(m_taskCompletedToken);
        }
    }

    void Scenario3_BackgroundTask::RegisterBackgroundTask(IInspectable const&, RoutedEventArgs const&)
    {
        // Register the background task without checking whether the user has enabled background execution.
        // If it's disabled, and then the user later enables background execution, the background task will be ready to run.
        // Create a new background task builder
        BackgroundTaskBuilder geolocTaskBuilder;

        geolocTaskBuilder.Name(BackgroundTaskName);
        geolocTaskBuilder.TaskEntryPoint(BackgroundTaskEntryPoint);

        // Create a new timer triggering at a 15 minute interval
        TimeTrigger trigger(15, false);

        // Associate the timer trigger with the background task builder
        geolocTaskBuilder.SetTrigger(trigger);

        // Register the background task
        m_geolocTask = geolocTaskBuilder.Register();

        // Associate an event handler with the new background task
        m_taskCompletedToken = m_geolocTask.Completed({ get_weak(), &Scenario3_BackgroundTask::OnCompleted });

        UpdateButtonStates();
        CheckBackgroundAndRequestLocationAccess();
    }

    void Scenario3_BackgroundTask::UnregisterBackgroundTask(IInspectable const&, RoutedEventArgs const&)
    {
        // Unregister the background task
        if (m_geolocTask != nullptr)
        {
            m_geolocTask.Unregister(true);
            m_geolocTask = nullptr;
        }

        ScenarioOutput_Latitude().Text(L"No data");
        ScenarioOutput_Longitude().Text(L"No data");
        ScenarioOutput_Accuracy().Text(L"No data");
        UpdateButtonStates();
        m_rootPage.NotifyUser(L"Background task unregistered", NotifyType::StatusMessage);
    }

    // This event handler is called when the background task is completed.
    fire_and_forget Scenario3_BackgroundTask::OnCompleted(IBackgroundTaskRegistration sender, BackgroundTaskCompletedEventArgs e)
    {
        auto lifetime = get_strong();

        // Update the UI with progress reported by the background task
        co_await resume_foreground(Dispatcher());

        try
        {
            // Rethrow any exception that occurred in the background task.
            e.CheckResult();

            // Update the UI with the completion status of the background task
            // The Run method of the background task sets this status.
            ReportSavedStatus();

            // Extract and display location data set by the background task if not null
            auto values = ApplicationData::Current().LocalSettings().Values();
            ScenarioOutput_Latitude().Text(unbox_value_or(values.TryLookup(L"Latitude"), L"No data"));
            ScenarioOutput_Longitude().Text(unbox_value_or(values.TryLookup(L"Longitude"), L"No data"));
            ScenarioOutput_Accuracy().Text(unbox_value_or(values.TryLookup(L"Accuracy"), L"No data"));
        }
        catch (...)
        {
            // The background task had an error
            m_rootPage.NotifyUser(to_message(), NotifyType::ErrorMessage);
        }
    }

    // Update the enable state of the register/unregister buttons
    // based on whether we have already registered a task.
    void Scenario3_BackgroundTask::UpdateButtonStates()
    {
        bool registered = (m_geolocTask != nullptr);
        RegisterBackgroundTaskButton().IsEnabled(!registered);
        UnregisterBackgroundTaskButton().IsEnabled(registered);
    }
}
