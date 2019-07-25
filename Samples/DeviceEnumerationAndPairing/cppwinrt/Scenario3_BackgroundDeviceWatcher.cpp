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
#include "Scenario3_BackgroundDeviceWatcher.h"
#include "SampleConfiguration.h"
#include "DeviceHelpers.h"
#include "DeviceWatcherHelper.h"
#include "Scenario3_BackgroundDeviceWatcher.g.cpp"

using namespace winrt;
using namespace winrt::Windows::ApplicationModel::Background;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

winrt::hstring backgroundTaskName = L"DeviceEnumeration_BackgroundTaskName";

namespace winrt::SDKTemplate::implementation
{
    Scenario3_BackgroundDeviceWatcher::Scenario3_BackgroundDeviceWatcher()
    {
        InitializeComponent();
    }

    void Scenario3_BackgroundDeviceWatcher::OnNavigatedTo(NavigationEventArgs const&)
    {
        selectorComboBox().ItemsSource(DeviceSelectorChoices::BackgroundDeviceWatcherSelectors());
        selectorComboBox().SelectedIndex(0);

        // Determine if the background task is already active
        for (auto&& task : BackgroundTaskRegistration::AllTasks())
        {
            if (backgroundTaskName == task.Value().Name())
            {
                backgroundTaskRegistration = task.Value();
                startWatcherButton().IsEnabled(false);
                stopWatcherButton().IsEnabled(true);
                break;
            }
        }
    }

    void Scenario3_BackgroundDeviceWatcher::OnNavigatedFrom(NavigationEventArgs const&)
    {
        if (backgroundTaskRegistration != nullptr)
        {
            backgroundTaskRegistration.Completed(taskCompletedToken);
            backgroundTaskRegistration = nullptr;
        }
    }

    void Scenario3_BackgroundDeviceWatcher::StartWatcherButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        StartWatcher();
    }

    void Scenario3_BackgroundDeviceWatcher::StopWatcherButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        StopWatcher();
    }

    void Scenario3_BackgroundDeviceWatcher::StartWatcher()
    {
        std::vector<DeviceWatcherEventKind> triggerEventKinds = { DeviceWatcherEventKind::Add, DeviceWatcherEventKind::Remove, DeviceWatcherEventKind::Update };

        startWatcherButton().IsEnabled(false);

        // First get the device selector chosen by the UI.
        IInspectable item = selectorComboBox().SelectedItem();
        DeviceSelectorInfo* deviceSelectorInfo = FromInspectable<DeviceSelectorInfo>(item);

        DeviceWatcher deviceWatcher{ nullptr };
        if (deviceSelectorInfo->Selector().empty())
        {
            // If a pre-canned device class selector was chosen, call the DeviceClass overload
            deviceWatcher = DeviceInformation::CreateWatcher(deviceSelectorInfo->DeviceClassSelector());
        }
        else if (deviceSelectorInfo->Kind() == DeviceInformationKind::Unknown)
        {
            // Use AQS string selector from dynamic call to a device api's GetDeviceSelector call
            // Kind will be determined by the selector
            deviceWatcher = DeviceInformation::CreateWatcher(
                deviceSelectorInfo->Selector(),
                nullptr // don't request additional properties for this sample
            );
        }
        else
        {
            // Kind is specified in the selector info
            deviceWatcher = DeviceInformation::CreateWatcher(
                deviceSelectorInfo->Selector(),
                nullptr, // don't request additional properties for this sample
                deviceSelectorInfo->Kind());
        }

        //
        // Get the background trigger for this watcher
        DeviceWatcherTrigger deviceWatcherTrigger = deviceWatcher.GetBackgroundTrigger(triggerEventKinds);

        rootPage.NotifyUser(L"Starting Watcher...", NotifyType::StatusMessage);

        // Register this trigger for our background task
        RegisterBackgroundTask(deviceWatcherTrigger);

        stopWatcherButton().IsEnabled(true);
    }

    void Scenario3_BackgroundDeviceWatcher::RegisterBackgroundTask(DeviceWatcherTrigger const& deviceWatcherTrigger)
    {
        // First see if we already have this background task registered. If so, unregister
        // it before we register it with the new trigger.

        for (auto&& task : BackgroundTaskRegistration::AllTasks())
        {
            if (backgroundTaskName == task.Value().Name())
            {
                UnregisterBackgroundTask(task.Value());
            }
        }

        BackgroundTaskBuilder taskBuilder;
        taskBuilder.Name(backgroundTaskName);
        taskBuilder.TaskEntryPoint(L"BackgroundDeviceWatcherTask.BackgroundDeviceWatcher");
        taskBuilder.SetTrigger(deviceWatcherTrigger);

        backgroundTaskRegistration = taskBuilder.Register();
        taskCompletedToken = backgroundTaskRegistration.Completed({ get_weak(), &Scenario3_BackgroundDeviceWatcher::OnTaskCompleted });
    }

    fire_and_forget Scenario3_BackgroundDeviceWatcher::OnTaskCompleted(BackgroundTaskRegistration const&, BackgroundTaskCompletedEventArgs const&)
    {
        co_await resume_foreground(Dispatcher());

        rootPage.NotifyUser(L"Background task completed", NotifyType::StatusMessage);
    }

    void Scenario3_BackgroundDeviceWatcher::StopWatcher()
    {
        stopWatcherButton().IsEnabled(false);
        if (backgroundTaskRegistration != nullptr)
        {
            UnregisterBackgroundTask(backgroundTaskRegistration);
        }
        startWatcherButton().IsEnabled(true);
    }

    void Scenario3_BackgroundDeviceWatcher::UnregisterBackgroundTask(IBackgroundTaskRegistration const& taskReg)
    {
        taskReg.Unregister(true);
        ApplicationDataContainer settings = ApplicationData::Current().LocalSettings();
        settings.Values().Insert(L"eventCount", box_value(0U));
    }
}
