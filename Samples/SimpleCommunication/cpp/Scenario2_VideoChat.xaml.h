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

#include "Utils.h"
#include "Scenario2_VideoChat.g.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_VideoChat sealed
    {
    public:
        Scenario2_VideoChat();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void CallButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void EndCallButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        task<void> Initialize();
        task<void> OnInitializeCompleted(Windows::Foundation::HResult hr);
        void OnInitializeCompletedDispatchedHandler();
        task<void> Deactivate();
        task<void> CleanUp();
        task<void> StartRecordingToCustomSinkAsync();
        task<void> EndCall();

        Windows::Foundation::EventRegistrationToken _incomingConnectionEventRegistrationToken;
        Windows::Foundation::EventRegistrationToken _mediaCaptureFailedEventRegistrationToken;
        Windows::Foundation::EventRegistrationToken _mediaElementFailedEventRegistrationToken;
        void OnIncomingConnection(Microsoft::Samples::SimpleCommunication::StspMediaSinkProxy^ sender, Microsoft::Samples::SimpleCommunication::IncomingConnectionEventArgs^ args);
        void OnMediaCaptureFailed(CaptureDevice^ sender, CaptureFailedEventArgs^ errorEventArgs);
        void VideoPlaybackErrorHandler(Platform::Object^ sender, Windows::UI::Xaml::ExceptionRoutedEventArgs^ e);

        void LockScenarioChange();
        void UnlockScenarioChange();

        bool _isTerminator;
        bool _initialization;
        CaptureDevice^ _device;
        bool _active;

        enum class Role
        {
            uninitialized,
            active,
            passive,
        };
        Role _role;

        SdkSample _sdkSample;
        Windows::UI::Core::CoreDispatcher^ _dispatcher;
        long volatile _navigationDisabled;
    };
}
