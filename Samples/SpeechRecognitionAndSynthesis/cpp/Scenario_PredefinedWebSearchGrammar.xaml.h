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

#include "Scenario_PredefinedWebSearchGrammar.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario_PredefinedWebSearchGrammar sealed
    {
    public:
        Scenario_PredefinedWebSearchGrammar();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        static const unsigned int HResultPrivacyStatementDeclined = 0x80045509;

        SDKTemplate::MainPage^ rootPage;
        Windows::UI::Core::CoreDispatcher^ dispatcher;
        Windows::Media::SpeechRecognition::SpeechRecognizer^ speechRecognizer;
        Windows::ApplicationModel::Resources::Core::ResourceContext^ speechContext;
        Windows::ApplicationModel::Resources::Core::ResourceMap^ speechResourceMap;

        void RecognizeWithUIWebSearchGrammar_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void RecognizeWithoutUIWebSearchGrammar_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void InitializeRecognizer(Windows::Globalization::Language^ recognizerLanguage);
        void PopulateLanguageDropdown();

        Windows::Foundation::EventRegistrationToken stateChangedToken;
        void SpeechRecognizer_StateChanged(Windows::Media::SpeechRecognition::SpeechRecognizer ^sender, Windows::Media::SpeechRecognition::SpeechRecognizerStateChangedEventArgs ^args);
        void cbLanguageSelection_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
        void openPrivacySettings_Click(Windows::UI::Xaml::Documents::Hyperlink^ sender, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs^ args);
    };

}
