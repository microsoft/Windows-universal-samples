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

#include "Scenario_ContinuousRecognitionListGrammar.g.h"
#include "MainPage.xaml.h"
#include <sstream>

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario_ContinuousRecognitionListGrammar sealed
    {
    public:
        Scenario_ContinuousRecognitionListGrammar();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        static const unsigned int HResultPrivacyStatementDeclined = 0x80045509;
        static const unsigned int HResultRecognizerNotFound = 0x8004503a;

        SDKTemplate::MainPage^ rootPage;
        Windows::Media::SpeechRecognition::SpeechRecognizer^ speechRecognizer;
        Windows::ApplicationModel::Resources::Core::ResourceContext^ speechContext;
        Windows::ApplicationModel::Resources::Core::ResourceMap^ speechResourceMap;
        bool isPopulatingLanguages = false;

        void ContinuousRecognize_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void InitializeRecognizer(Windows::Globalization::Language^ recognizerLanguage);
        void PopulateLanguageDropdown();

        Windows::Foundation::EventRegistrationToken stateChangedToken;
        Windows::Foundation::EventRegistrationToken continuousRecognitionCompletedToken;
        Windows::Foundation::EventRegistrationToken continuousRecognitionResultGeneratedToken;

        void SpeechRecognizer_StateChanged(Windows::Media::SpeechRecognition::SpeechRecognizer ^sender, Windows::Media::SpeechRecognition::SpeechRecognizerStateChangedEventArgs ^args);
        void ContinuousRecognitionSession_Completed(Windows::Media::SpeechRecognition::SpeechContinuousRecognitionSession ^sender, Windows::Media::SpeechRecognition::SpeechContinuousRecognitionCompletedEventArgs ^args);
        void ContinuousRecognitionSession_ResultGenerated(Windows::Media::SpeechRecognition::SpeechContinuousRecognitionSession ^sender, Windows::Media::SpeechRecognition::SpeechContinuousRecognitionResultGeneratedEventArgs ^args);
        void cbLanguageSelection_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
    };
}
