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

#include "Scenario1_LowLatency.g.h"
#include "Utils.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_LowLatency sealed
    {
    public:
        Scenario1_LowLatency();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

        void VisibilityChanged(Object^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ e);    
    
    private:
        task<void> Initialize();
        void InitializeUIControls();
        void Reset();
        void UpdateLocalClientWindow();
        task<void> Cleanup();
        task<void> StartRecordingToCustomSink();

        VideoSettingsFilter^ _videoSettingsFilter;

        Windows::Foundation::EventRegistrationToken _incomingConnectionEventRegistrationToken;
        Windows::Foundation::EventRegistrationToken _mediaCaptureFailedEventRegistrationToken;
        void OnMediaCaptureFailed(CaptureDevice^ sender, CaptureFailedEventArgs^ errorEventArgs);
        void OnIncomingConnection(Microsoft::Samples::SimpleCommunication::StspMediaSinkProxy^ sender, Microsoft::Samples::SimpleCommunication::IncomingConnectionEventArgs^ args);
        void HandleError(Platform::Exception^ error, Platform::String^ msg, Platform::String^ prefix);

        enum class ScenarioOneStates
        {
            uninitialized,
            initializing,
            waiting,
            previewing,
            streaming,
            end,
        };
        ScenarioOneStates _currentState;
        ScenarioOneStates _previousState;

        /// This is the FSM encapsulating the state table (states and actions) of the scenario.
        delegate IAsyncAction^ ActionTransition();
        std::map<ScenarioOneStates, std::array<ActionTransition^, 5>> actionTransitionFunctions;
        IAsyncAction^ initializing_previewSetupCompleted();
        IAsyncAction^ waiting_previewButtonClicked();
        IAsyncAction^ previewing_previewButtonClicked();
        IAsyncAction^ previewing_localClientButtonClicked();
        IAsyncAction^ previewing_recordingStarted();
        IAsyncAction^ streaming_previewButtonClicked();
        IAsyncAction^ streaming_latencyModeToggled();
        IAsyncAction^ streaming_recordingStarted();

        enum class LatencyModes
        {
            uninitialized,
            highLatency,
            lowLatency,
        };
        LatencyModes _latencyMode;
        std::map<LatencyModes, Windows::UI::Xaml::Controls::MediaElement^> _localHostVideo;

        value struct StreamFilteringCriteria
        {
            Platform::String^   aspectRatio;
            int                 horizontalResolution;
            Platform::String^   subType;
        };
        StreamFilteringCriteria _streamFilteringCriteria;

        bool _initialized;
        CaptureDevice^ _device;

        void PreviewButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void LocalClientButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void LatencyModeToggled(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        SdkSample _sdkSample;
        Windows::UI::Core::CoreDispatcher^ _dispatcher;
        Windows::Foundation::EventRegistrationToken m_visibilityEventRegistrationToken;

        void OnMediaFailed(Platform::Object^ sender, Windows::UI::Xaml::ExceptionRoutedEventArgs^ e);

        void LockScenarioChange();
        void UnlockScenarioChange();

        long volatile _navigationDisabled;
    };
}
