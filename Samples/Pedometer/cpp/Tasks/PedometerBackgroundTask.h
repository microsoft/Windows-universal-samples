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
// PedometerBackgroundTask.h
// Declaration of the PedometerBackgroundTask class
//

#pragma once

namespace Tasks
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class PedometerBackgroundTask sealed : public Windows::ApplicationModel::Background::IBackgroundTask
    {

    public:
        PedometerBackgroundTask() {}

        virtual void Run(Windows::ApplicationModel::Background::IBackgroundTaskInstance^ taskInstance);
        void OnCanceled(Windows::ApplicationModel::Background::IBackgroundTaskInstance^ taskInstance, Windows::ApplicationModel::Background::BackgroundTaskCancellationReason reason);

    private:
        ~PedometerBackgroundTask() {}

        Windows::ApplicationModel::Background::IBackgroundTaskInstance^ TaskInstance;
    };
}