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
#include "Scenario6_CustomTrigger.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

// {C7BDD245-5CDA-4BBD-B68D-B5E36F7911A3}
#define GUID_CUSTOMSYSTEMEVENTTRIGGERID_OSRUSBFX2 L"{C7BDD245-5CDA-4BBD-B68D-B5E36F7911A3}"

static String^ customTriggerTaskName = L"Osrusbfx2Task.ConnectedTask";

CustomTrigger::CustomTrigger()
{
    InitializeComponent();
    UpdateUI();
}

void CustomTrigger::OnNavigatedFrom(NavigationEventArgs^ e)
{
    UnregisterTask();
}

void CustomTrigger::UpdateUI()
{
    RegisterCustomTrigger->IsEnabled = (taskRegistration == nullptr);
    UnregisterCustomTrigger->IsEnabled = (taskRegistration != nullptr);
}

void CustomTrigger::UnregisterTask()
{
    if (taskRegistration != nullptr)
    {
        taskRegistration->Unregister(true);
        taskRegistration = nullptr;
    }
}


void CustomTrigger::RegisterCustomTrigger_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (IsFx2CustomTriggerTaskRegistered())
    {
        rootPage->NotifyUser("Osrusbfx2.ConnectedTask background task has already been registered", NotifyType::ErrorMessage);
        return;
    }

    // Create a new background task builder.
    BackgroundTaskBuilder^ taskBuilder = ref new BackgroundTaskBuilder();

    // Create a new OEM trigger.
    CustomSystemEventTrigger^ fx2Trigger = ref new CustomSystemEventTrigger(
        GUID_CUSTOMSYSTEMEVENTTRIGGERID_OSRUSBFX2, // Trigger Qualifier
        CustomSystemEventTriggerRecurrence::Once); // One-shot trigger

    // Associate the fx2Trigger trigger with the background task builder.
    taskBuilder->SetTrigger(fx2Trigger);

    // Specify the background task to run when the trigger fires.
    taskBuilder->TaskEntryPoint = customTriggerTaskName;

    // Name the background task.
    taskBuilder->Name = customTriggerTaskName;

    try
    {
        // Register the background task.
        taskRegistration = taskBuilder->Register();
    }
    catch (Exception^ ex)
    {
        rootPage->NotifyUser(ex->ToString(), NotifyType::ErrorMessage);
        return;
    }

    UpdateUI();
}

bool CustomTrigger::IsFx2CustomTriggerTaskRegistered()
{
    for (auto task : BackgroundTaskRegistration::AllTasks)
    {
        String^ taskName = task->Value->Name;
        if (taskName == customTriggerTaskName)
        {
            return true;
        }
    }

    return false;
}

void CustomTrigger::UnregisterCustomTrigger_Click(Object^ sender, RoutedEventArgs^ e)
{
    UnregisterTask();
    UpdateUI();
}
