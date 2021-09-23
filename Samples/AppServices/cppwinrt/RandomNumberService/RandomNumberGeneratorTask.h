#pragma once

#include "RandomNumberGeneratorTask.g.h"
#include <random>

namespace winrt::RandomNumberService::implementation
{
    struct RandomNumberGeneratorTask : RandomNumberGeneratorTaskT<RandomNumberGeneratorTask>
    {
        RandomNumberGeneratorTask() = default;

        void Run(Windows::ApplicationModel::Background::IBackgroundTaskInstance const& taskInstance);

    private:
        Windows::ApplicationModel::Background::BackgroundTaskDeferral serviceDeferral{ nullptr };
        Windows::ApplicationModel::AppService::AppServiceConnection connection{ nullptr };
        std::mt19937 randomNumberGenerator;

        void OnTaskCanceled(Windows::ApplicationModel::Background::IBackgroundTaskInstance const&, Windows::ApplicationModel::Background::BackgroundTaskCancellationReason const&);
        fire_and_forget OnRequestReceived(Windows::ApplicationModel::AppService::AppServiceConnection const&, Windows::ApplicationModel::AppService::AppServiceRequestReceivedEventArgs const& args);
    };
}

namespace winrt::RandomNumberService::factory_implementation
{
    struct RandomNumberGeneratorTask : RandomNumberGeneratorTaskT<RandomNumberGeneratorTask, implementation::RandomNumberGeneratorTask>
    {
    };
}
