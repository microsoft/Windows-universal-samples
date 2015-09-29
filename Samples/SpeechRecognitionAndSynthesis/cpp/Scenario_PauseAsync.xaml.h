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

#include "Scenario_PauseAsync.g.h"
#include "MainPage.xaml.h"
#include <sstream>

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario_PauseAsync sealed
    {
    public:
        Scenario_PauseAsync();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        static const unsigned int HResultPrivacyStatementDeclined = 0x80045509;
        Platform::String^ newButtonText;

        SDKTemplate::MainPage^ rootPage;
        Windows::UI::Core::CoreDispatcher^ dispatcher;
        Windows::Media::SpeechRecognition::SpeechRecognizer^ speechRecognizer;
        Windows::Media::SpeechRecognition::SpeechRecognitionListConstraint^ emailConstraint;
        Windows::Media::SpeechRecognition::SpeechRecognitionListConstraint^ phoneConstraint;

        void Recognize_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void EmailButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void PhoneButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void InitializeRecognizer();
        void EnableUI(bool newState);

        Windows::Foundation::EventRegistrationToken stateChangedToken;
        Windows::Foundation::EventRegistrationToken continuousRecognitionCompletedToken;
        Windows::Foundation::EventRegistrationToken continuousRecognitionResultGeneratedToken;

        void SpeechRecognizer_StateChanged(Windows::Media::SpeechRecognition::SpeechRecognizer ^sender, Windows::Media::SpeechRecognition::SpeechRecognizerStateChangedEventArgs ^args);
        void ContinuousRecognitionSession_Completed(Windows::Media::SpeechRecognition::SpeechContinuousRecognitionSession ^sender, Windows::Media::SpeechRecognition::SpeechContinuousRecognitionCompletedEventArgs ^args);
        void ContinuousRecognitionSession_ResultGenerated(Windows::Media::SpeechRecognition::SpeechContinuousRecognitionSession ^sender, Windows::Media::SpeechRecognition::SpeechContinuousRecognitionResultGeneratedEventArgs ^args);
    };
}
