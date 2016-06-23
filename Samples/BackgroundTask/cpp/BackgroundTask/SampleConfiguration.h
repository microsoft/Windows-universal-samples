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

using namespace Platform;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::Storage;

#define SampleBackgroundTaskEntryPoint "Tasks.SampleBackgroundTask"
#define SampleBackgroundTaskName "SampleBackgroundTask"
#define SampleBackgroundTaskWithConditionName "SampleBackgroundTaskWithCondition"
#define ServicingCompleteTaskEntryPoint "Tasks.ServicingComplete"
#define ServicingCompleteTaskName "ServicingCompleteTask"
#define TimeTriggeredTaskName "TimeTriggeredTask"
#define ApplicationTriggerTaskName "ApplicationTriggerTask"

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
                return L"Background tasks";
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
        static BackgroundTaskRegistration^ RegisterBackgroundTask(String^ taskEntryPoint, String^ name, IBackgroundTrigger^ trigger, IBackgroundCondition^ condition);
        static bool TaskRequiresBackgroundAccess(String^ name);
        static void UnregisterBackgroundTasks(String^ name);
        static void UpdateBackgroundTaskRegistrationStatus(String^ name, bool registered);

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
    };

}
