// Copyright (c) Microsoft Corporation. All rights reserved.

#pragma once

using namespace Windows::ApplicationModel::Background;

namespace BackgroundDeviceWatcherTaskCpp
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class BackgroundDeviceWatcher sealed : public IBackgroundTask
    {
    public:
        BackgroundDeviceWatcher() {}

        virtual void Run(IBackgroundTaskInstance^ taskInstance);
    };
}
