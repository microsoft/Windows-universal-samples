#pragma once

#include "BackgroundDeviceWatcher.g.h"

namespace winrt::BackgroundDeviceWatcherTask::implementation
{
    struct BackgroundDeviceWatcher : BackgroundDeviceWatcherT<BackgroundDeviceWatcher>
    {
        BackgroundDeviceWatcher() = default;

        void Run(Windows::ApplicationModel::Background::IBackgroundTaskInstance const& taskInstance);
    };
}

namespace winrt::BackgroundDeviceWatcherTask::factory_implementation
{
    struct BackgroundDeviceWatcher : BackgroundDeviceWatcherT<BackgroundDeviceWatcher, implementation::BackgroundDeviceWatcher>
    {
    };
}
