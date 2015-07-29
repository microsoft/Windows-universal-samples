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

#include "Scenario_SynthesizeSSML.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario_SynthesizeSSML sealed
    {
    public:
        Scenario_SynthesizeSSML();
    private:
        SDKTemplate::MainPage^ rootPage;

        Windows::Media::SpeechSynthesis::SpeechSynthesizer^ synthesizer;
        Windows::ApplicationModel::Resources::Core::ResourceContext^ speechContext;
        Windows::ApplicationModel::Resources::Core::ResourceMap^ speechResourceMap;

        void Speak_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ListboxVoiceChooser_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
        void media_MediaEnded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void InitializeListboxVoiceChooser();
        void UpdateSSMLText();
    };
}
