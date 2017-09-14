#pragma once

namespace RandomNumberService
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class RandomNumberGeneratorTask sealed : Windows::ApplicationModel::Background::IBackgroundTask
    {
    public:
        virtual void Run(Windows::ApplicationModel::Background::IBackgroundTaskInstance^ taskInstance);
        void OnTaskCanceled(Windows::ApplicationModel::Background::IBackgroundTaskInstance^ sender, Windows::ApplicationModel::Background::BackgroundTaskCancellationReason reason);
    };
}