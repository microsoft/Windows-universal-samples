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
#include "SampleConfiguration.h"
#include "Scenario8_VisitsBackgroundTask.h"
#include "Scenario8_VisitsBackgroundTask.g.cpp"

using namespace winrt;
using namespace winrt::Windows::ApplicationModel::Background;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Devices::Geolocation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario8_VisitsBackgroundTask::Scenario8_VisitsBackgroundTask()
    {
        InitializeComponent();
    }

    void Scenario8_VisitsBackgroundTask::OnNavigatedTo(NavigationEventArgs const&)
    {
        // See if our background task is already registered.
        m_visitTask = LookupBackgroundTask(BackgroundTaskName);

        if (m_visitTask != nullptr)
        {
            // Associate an event handler with the existing background task
            m_taskCompletedToken = m_visitTask.Completed({ get_weak(), &Scenario8_VisitsBackgroundTask::OnCompleted });
        }
        FillEventListBoxWithExistingEvents();
        UpdateButtonStates();
    }

    void Scenario8_VisitsBackgroundTask::RegisterBackgroundTask(IInspectable const&, RoutedEventArgs const&)
    {
        // Register the background task without checking whether the user has enabled background execution.
        // If it's disabled, and then the user later enables background execution, the background task will be ready to run.
        // Create a new background task builder
        BackgroundTaskBuilder visitTaskBuilder;

        visitTaskBuilder.Name(BackgroundTaskName);
        visitTaskBuilder.TaskEntryPoint(BackgroundTaskEntryPoint);

        // Create a new location trigger
        GeovisitTrigger trigger;

        // Set the desired monitoring scope.
        // For higher granularity such as venue/building level changes, choose venue.
        // For lower granularity more or less in the range of zipcode level changes, choose city.
        // Choosing Venue here as an example.
        trigger.MonitoringScope(VisitMonitoringScope::Venue);

        // Associate the trigger with the background task builder
        visitTaskBuilder.SetTrigger(trigger);

        // Register the background task
        m_visitTask = visitTaskBuilder.Register();

        // Associate an event handler with the new background task
        m_taskCompletedToken = m_visitTask.Completed({ get_weak(), &Scenario8_VisitsBackgroundTask::OnCompleted });

        UpdateButtonStates();
        CheckBackgroundAndRequestLocationAccess();
    }

    void Scenario8_VisitsBackgroundTask::UnregisterBackgroundTask(IInspectable const&, RoutedEventArgs const&)
    {
        // Unregister the background task
        if (m_visitTask != nullptr)
        {
            m_visitTask.Unregister(true);
            m_visitTask.Completed(m_taskCompletedToken);
            m_visitTask = nullptr;
        }

        m_rootPage.NotifyUser(L"Geofence background task unregistered", NotifyType::StatusMessage);
        UpdateButtonStates();
    }

    // This event handler is called when the background task is completed.
    fire_and_forget Scenario8_VisitsBackgroundTask::OnCompleted(IBackgroundTaskRegistration sender, BackgroundTaskCompletedEventArgs e)
    {
        auto lifetime = get_strong();

        // Update the UI with progress reported by the background task
        co_await resume_foreground(Dispatcher());

        try
        {
            // Rethrow any exception that occurred in the background task.
            e.CheckResult();

            // Update the UI with the completion status of the background task
            ReportSavedStatus();
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
    void Scenario8_VisitsBackgroundTask::UpdateButtonStates()
    {
        bool registered = (m_visitTask != nullptr);
        RegisterBackgroundTaskButton().IsEnabled(!registered);
        UnregisterBackgroundTaskButton().IsEnabled(registered);
    }

    void Scenario8_VisitsBackgroundTask::FillEventListBoxWithExistingEvents()
    {
        FillItemsFromSavedJson(VisitBackgroundEventsListBox(), L"BackgroundVisitEventCollection");
    }
}
