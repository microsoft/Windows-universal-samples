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
#pragma once
namespace VoiceCommandService
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class AdventureWorksVoiceCommandService sealed : public Windows::ApplicationModel::Background::IBackgroundTask
    {
    public:
        // Inherited via IBackgroundTask
        virtual void Run(Windows::ApplicationModel::Background::IBackgroundTaskInstance ^taskInstance);

    private:
        Windows::ApplicationModel::Resources::Core::ResourceContext^ context;
        Windows::ApplicationModel::Resources::Core::ResourceMap^ resourceMap;

        Windows::Foundation::EventRegistrationToken taskCancelledToken;
        Windows::ApplicationModel::VoiceCommands::VoiceCommandServiceConnection^ voiceServiceConnection;
        Platform::Agile<Windows::ApplicationModel::Background::BackgroundTaskDeferral^> serviceDeferral;
        void OnCanceled(
            Windows::ApplicationModel::Background::IBackgroundTaskInstance ^sender,
            Windows::ApplicationModel::Background::BackgroundTaskCancellationReason reason);
        void OnVoiceCommandCompleted(
            Windows::ApplicationModel::VoiceCommands::VoiceCommandServiceConnection ^sender,
            Windows::ApplicationModel::VoiceCommands::VoiceCommandCompletedEventArgs ^args);

        void SendCompletionMessageForDestination(Platform::String^ destination);
        void SendCompletionMessageForCancellation(Platform::String^ destination);
        AdventureWorks_Shared::Trip^ DisambiguateTrips(std::vector<AdventureWorks_Shared::Trip^> trips, Platform::String^ disambiguationMessage, Platform::String^ secondDisambiguationMessage);

        void LaunchAppInForeground();

        void ShowProgressScreen(Platform::String^ progressMessage);

        Platform::String^ GetResourceString(Platform::String^ resourceName);
        Platform::String^ GetResourceStringAndFormat(Platform::String^ resourceName, Platform::String^ param);
    };
}
