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

#include "pch.h"
#include <agile.h>

using namespace Windows::ApplicationModel::Background;
using namespace Windows::System::Threading;

namespace Tasks
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class SampleBackgroundTask sealed : public IBackgroundTask
    {

    public:
        SampleBackgroundTask();

        virtual void Run(IBackgroundTaskInstance^ taskInstance);
        void OnCanceled(IBackgroundTaskInstance^ taskInstance, BackgroundTaskCancellationReason reason);

    private:
        ~SampleBackgroundTask();

        BackgroundTaskCancellationReason CancelReason;
        volatile bool CancelRequested;
        Platform::Agile<Windows::ApplicationModel::Background::BackgroundTaskDeferral> TaskDeferral;
        ThreadPoolTimer^ PeriodicTimer;
        unsigned int Progress;
        IBackgroundTaskInstance^ TaskInstance;
    };
}