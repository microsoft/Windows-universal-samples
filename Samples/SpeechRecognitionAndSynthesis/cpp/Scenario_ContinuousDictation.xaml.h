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

#include "Scenario_ContinuousDictation.g.h"
#include "MainPage.xaml.h"
#include <sstream>

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario_ContinuousDictation sealed
    {
    public:
        Scenario_ContinuousDictation();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        static const unsigned int HResultPrivacyStatementDeclined = 0x80045509;

        SDKTemplate::MainPage^ rootPage;
        Windows::Media::SpeechRecognition::SpeechRecognizer^ speechRecognizer;
        std::wstringstream dictatedTextBuilder;

        void ContinuousRecognize_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void btnClearText_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void dictationTextBox_TextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e);

        void InitializeRecognizer(Windows::Globalization::Language^ recognizerLanguage);
        void PopulateLanguageDropdown();

        Windows::Foundation::EventRegistrationToken stateChangedToken;
        Windows::Foundation::EventRegistrationToken continuousRecognitionCompletedToken;
        Windows::Foundation::EventRegistrationToken continuousRecognitionResultGeneratedToken;
        Windows::Foundation::EventRegistrationToken hypothesisGeneratedToken;

        void SpeechRecognizer_StateChanged(Windows::Media::SpeechRecognition::SpeechRecognizer ^sender, Windows::Media::SpeechRecognition::SpeechRecognizerStateChangedEventArgs ^args);
        void ContinuousRecognitionSession_Completed(Windows::Media::SpeechRecognition::SpeechContinuousRecognitionSession ^sender, Windows::Media::SpeechRecognition::SpeechContinuousRecognitionCompletedEventArgs ^args);
        void ContinuousRecognitionSession_ResultGenerated(Windows::Media::SpeechRecognition::SpeechContinuousRecognitionSession ^sender, Windows::Media::SpeechRecognition::SpeechContinuousRecognitionResultGeneratedEventArgs ^args);
        void SpeechRecognizer_HypothesisGenerated(Windows::Media::SpeechRecognition::SpeechRecognizer ^sender, Windows::Media::SpeechRecognition::SpeechRecognitionHypothesisGeneratedEventArgs ^args);
        void cbLanguageSelection_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
        void openPrivacySettings_Click(Windows::UI::Xaml::Documents::Hyperlink^ sender, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs^ args);
    };
}
