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

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class BackgroundActivity sealed
    {
    public:
        void Run(Windows::ApplicationModel::Background::IBackgroundTaskInstance^ taskInstance);
        void OnCanceled(Windows::ApplicationModel::Background::IBackgroundTaskInstance^ taskInstance, Windows::ApplicationModel::Background::BackgroundTaskCancellationReason reason);

        static void Start(Windows::ApplicationModel::Background::IBackgroundTaskInstance^ taskInstance);
    private:
        Windows::ApplicationModel::Background::BackgroundTaskCancellationReason CancelReason = Windows::ApplicationModel::Background::BackgroundTaskCancellationReason::Abort;
        volatile bool CancelRequested = false;
        Platform::Agile<Windows::ApplicationModel::Background::BackgroundTaskDeferral> TaskDeferral = nullptr;
        Windows::System::Threading::ThreadPoolTimer^ PeriodicTimer = nullptr;
        unsigned int Progress = 0;
        Windows::ApplicationModel::Background::IBackgroundTaskInstance^ TaskInstance = nullptr;
    };
}
