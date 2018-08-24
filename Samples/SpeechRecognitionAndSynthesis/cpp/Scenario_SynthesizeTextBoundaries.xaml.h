#pragma once
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

#include "Scenario_SynthesizeTextBoundaries.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario_SynthesizeTextBoundaries sealed
    {
    public:
        Scenario_SynthesizeTextBoundaries();
    private:
        SDKTemplate::MainPage^ rootPage;
        

        Windows::Media::SpeechSynthesis::SpeechSynthesizer^ synthesizer;
        Windows::Media::SpeechSynthesis::SpeechSynthesisStream^ synthesisStream;
        Windows::ApplicationModel::Resources::Core::ResourceContext^ speechContext;
        Windows::ApplicationModel::Resources::Core::ResourceMap^ speechResourceMap;

        void Speak_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ListboxVoiceChooser_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
        void media_MediaEnded(Windows::Media::Playback::MediaPlayer^ sender, Platform::Object^ e);
        Concurrency::task <void> SynthesizeTextStream();
        void RegisterForBoundaryEvents(Windows::Media::Playback::MediaPlaybackItem^ mediaPlaybackItem);
        void RegisterMetadataHandlerForWords(Windows::Media::Playback::MediaPlaybackItem^ mediaPlaybackItem, int index);
        void RegisterMetadataHandlerForSentences(Windows::Media::Playback::MediaPlaybackItem^ mediaPlaybackItem, int index);
        void metadata_SpeechCueEntered(Windows::Media::Core::TimedMetadataTrack^ timedMetadataTrack, Windows::Media::Core::MediaCueEventArgs^ args);
        void HighlightWordOnScreen(int startPositionInInput, int endPositionInInput);
        void FillTextBoxes(Windows::Media::Core::SpeechCue^ cue, Windows::Media::Core::TimedMetadataTrack^ timedMetadataTrack);
        void timedMetadataTrackChangedHandler(Windows::Media::Playback::MediaPlaybackItem^ mediaPlaybackItem, 
                                              Windows::Foundation::Collections::IVectorChangedEventArgs^ args);

        void InitializeListboxVoiceChooser();
    };
}
