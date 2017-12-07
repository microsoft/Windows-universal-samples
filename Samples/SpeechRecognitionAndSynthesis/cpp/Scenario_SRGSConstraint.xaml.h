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

#include "Scenario_SRGSConstraint.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario_SRGSConstraint sealed
    {
    public:
        Scenario_SRGSConstraint();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        /// <summary>
        /// This HResult represents the scenario where a user is prompted to allow in-app speech, but
        /// declines. This should only happen on a Phone device, where speech is enabled for the entire device,
        /// not per-app.
        /// </summary>
        static const unsigned int HResultPrivacyStatementDeclined = 0x80045509;
        static const unsigned int HResultRecognizerNotFound = 0x8004503a;

        SDKTemplate::MainPage^ rootPage;
        Windows::UI::Core::CoreDispatcher^ dispatcher;
        Windows::Media::SpeechRecognition::SpeechRecognizer^ speechRecognizer;
        Windows::Foundation::Collections::IMap<Platform::String^, Windows::UI::Color>^ colorLookup;
        Windows::ApplicationModel::Resources::Core::ResourceContext^ speechContext;
        Windows::ApplicationModel::Resources::Core::ResourceMap^ speechResourceMap;
        bool isPopulatingLanguages = false;

        void RecognizeWithUI_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void RecognizeWithoutUI_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void PopulateLanguageDropdown();
        void InitializeRecognizer(Windows::Globalization::Language^ recognizerLanguage);

        void HandleRecognitionResult(Windows::Media::SpeechRecognition::SpeechRecognitionResult^ recoResult);
        Windows::UI::Color getColor(Platform::String^ colorString);

        Windows::Foundation::EventRegistrationToken stateChangedToken;

        void SpeechRecognizer_StateChanged(Windows::Media::SpeechRecognition::SpeechRecognizer ^sender, Windows::Media::SpeechRecognition::SpeechRecognizerStateChangedEventArgs ^args);
        void cbLanguageSelection_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
    };
}
