//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

#include <collection.h>
#include "App.g.h"
#include "BackgroundActivity.h"

using namespace Platform;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Storage;

#define SampleBackgroundTaskName "SampleBackgroundTask"
#define SampleBackgroundTaskWithConditionName "SampleBackgroundTaskWithCondition"
#define ServicingCompleteTaskName "ServicingCompleteTask"
#define TimeTriggeredTaskName "TimeTriggeredTask"
#define ApplicationTriggerTaskName "ApplicationTriggerTask"
#define GroupedBackgroundTaskName "GroupedBackgroundTask"
#define BackgroundTaskGroupId "3F2504E0-5F89-41D3-9A0C-0405E82C3333"
#define BackgroundTaskGroupFriendlyName "Background Task Group"


namespace SDKTemplate
{
    public value struct Scenario
    {
        String^ Title;
        String^ ClassName;
    };

    partial ref class MainPage
    {
    public:
        static property String^ FEATURE_NAME
        {
            String^ get()
            {
                return L"Background Activation";
            }
        }

        static property Array<Scenario>^ scenarios
        {
            Array<Scenario>^ get()
            {
                return scenariosInner;
            }
        }
    private:
        static Array<Scenario>^ scenariosInner;
    };

    class BackgroundTaskSample
    {
    public:
        static String^ GetBackgroundTaskStatus(String^ name);
        static BackgroundTaskRegistration^ RegisterBackgroundTask(String^ taskEntryPoint, String^ name, IBackgroundTrigger^ trigger, IBackgroundCondition^ condition, BackgroundTaskRegistrationGroup^ group = nullptr);
        static bool TaskRequiresBackgroundAccess(String^ name);
        static void UnregisterBackgroundTasks(String^ name, BackgroundTaskRegistrationGroup^ group = nullptr);
        static BackgroundTaskRegistrationGroup^ GetTaskGroup(String^ id, String^ groupName);
        static void UpdateBackgroundTaskRegistrationStatus(String^ name, bool registered);
        static void RemoveBackgroundTaskStatus(String^ name);

        static String^ SampleBackgroundTaskProgress;
        static bool SampleBackgroundTaskRegistered;

        static String^ SampleBackgroundTaskWithConditionProgress;
        static bool SampleBackgroundTaskWithConditionRegistered;

        static String^ ServicingCompleteTaskProgress;
        static bool ServicingCompleteTaskRegistered;

        static String^ TimeTriggeredTaskProgress;
        static bool TimeTriggeredTaskRegistered;

        static String^ ApplicationTriggerTaskProgress;
        static bool ApplicationTriggerTaskRegistered;
        static String^ ApplicationTriggerTaskResult;

        static String^ GroupedBackgroundTaskProgress;
        static bool GroupedBackgroundTaskRegistered;

        static Windows::Foundation::Collections::PropertySet^ TaskStatuses;
    };
}


