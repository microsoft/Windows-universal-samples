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

#include "pch.h"
#include "Scenario_SynthesizeTextBoundaries.xaml.h"
#include <pplawait.h>

using namespace SDKTemplate;
using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Resources::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Media::Core;
using namespace Windows::Media::Playback;
using namespace Windows::Media::SpeechSynthesis;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Core;

Scenario_SynthesizeTextBoundaries::Scenario_SynthesizeTextBoundaries() : rootPage(MainPage::Current)
{
    InitializeComponent();
    synthesizer = ref new SpeechSynthesizer();

    // initialize localization for text blob
    speechContext = ResourceContext::GetForCurrentView();
    speechContext->Languages = ref new VectorView<String^>(1, SpeechSynthesizer::DefaultVoice->Language);

    speechResourceMap = ResourceManager::Current->MainResourceMap->GetSubtree(L"LocalizationTTSResources");

    InitializeListboxVoiceChooser();

    MediaPlayer^ player = ref new MediaPlayer();
    player->AutoPlay = false;
    player->MediaEnded += ref new  TypedEventHandler<MediaPlayer^, Object^>(this,&Scenario_SynthesizeTextBoundaries::media_MediaEnded);

    media->SetMediaPlayer(player);
}

/// <summary>
/// This is invoked when the user clicks on the speak/stop button.
/// </summary>
/// <param name="sender">Button that triggered this event</param>
/// <param name="e">State information about the routed event</param>
void Scenario_SynthesizeTextBoundaries::Speak_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // If the media is playing, the user has pressed the button to stop the playback.
    if (media->MediaPlayer->PlaybackSession->PlaybackState == MediaPlaybackState::Playing)
    {
        media->MediaPlayer->Pause();
        btnSpeak->Content = "Speak";
    }
    else
    {
        String^ text = textToSynthesize->Text;
        if (!text->IsEmpty())
        {
            // Change the button label. You could also just disable the button if you don't want any user control.
            btnSpeak->Content = "Stop";

            synthesizer->Options->IncludeWordBoundaryMetadata = true;
            synthesizer->Options->IncludeSentenceBoundaryMetadata = true;

            //functionality wrapped in task function
            SynthesizeTextStream();
        }
    }

}

task<void> Scenario_SynthesizeTextBoundaries::SynthesizeTextStream()
{
    String^ text = textToSynthesize->Text;

    return create_task(synthesizer->SynthesizeTextToStreamAsync(text))
        .then([this](task<SpeechSynthesisStream^> previousTask)
    {
        try
        {
            synthesisStream = previousTask.get();

            // Create a media source from the stream: 
            MediaSource^ mediaSource = MediaSource::CreateFromStream(synthesisStream, synthesisStream->ContentType);

            //Create a Media Playback Item   
            MediaPlaybackItem^ mediaPlaybackItem = ref new MediaPlaybackItem(mediaSource);

            // Ensure that the app is notified for cues.  
            RegisterForBoundaryEvents(mediaPlaybackItem);

            // Set the source of the MediaElement or MediaPlayerElement to the MediaPlaybackItem 
            // and start playing the synthesized audio stream.
            media->Source = mediaPlaybackItem;
            media->MediaPlayer->Play();
        }
        catch (Exception^ ex)
        {
            if (ex->HResult == HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND))
            {
                btnSpeak->Content = L"Speak";
                btnSpeak->IsEnabled = false;
                listboxVoiceChooser->IsEnabled = false;
                Windows::UI::Popups::MessageDialog^ dialog =
                    ref new Windows::UI::Popups::MessageDialog(ex->Message, "Media playback components unavailable.");
                create_task(dialog->ShowAsync());
            }
            else
            {
                // If the text is not able to be synthesized, throw an error message to the user.
                btnSpeak->Content = L"Speak";
                Windows::UI::Popups::MessageDialog^ dialog =
                    ref new Windows::UI::Popups::MessageDialog(ex->Message, "Unable to synthesize text");
                create_task(dialog->ShowAsync());
            }
        }
    }, task_continuation_context::use_current());
}

/// <summary>
/// This is called when the user has selects a voice from the drop down.
/// </summary>
/// <param name="sender">unused object parameter</param>
/// <param name="e">unused event parameter</param>
void Scenario_SynthesizeTextBoundaries::ListboxVoiceChooser_SelectionChanged(Object^ sender, SelectionChangedEventArgs^ e)
{
    if (e->AddedItems->Size > 0)
    {
        ComboBoxItem^ first = safe_cast<ComboBoxItem^>(e->AddedItems->GetAt(0));
        VoiceInformation^ voice = safe_cast<VoiceInformation^>(first->Tag);
        synthesizer->Voice = voice;


        // update UI text to be an appropriate default translation.
        speechContext->Languages = ref new VectorView<String^>(1, { voice->Language });
        textToSynthesize->Text = speechResourceMap->GetValue("SynthesizeTextBoundariesDefaultText", speechContext)->ValueAsString;

    }
}

/// <summary>
/// This is invoked when the stream is finished playing.
/// </summary>
/// <remarks>
/// In this case, we're changing the button label based on the state.
/// </remarks>
/// <param name="sender">unused object parameter</param>
/// <param name="e">unused event parameter</param>
void Scenario_SynthesizeTextBoundaries::media_MediaEnded(Windows::Media::Playback::MediaPlayer^ sender, Platform::Object^ e)
{

    // Because this call is not awaited, execution of the current method continues before the call is completed
    Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, 
                                                                                                 ref new Windows::UI::Core::DispatchedHandler([this]()
    {
        btnSpeak->Content = "Speak";
    }));
}

/// <summary>
/// This creates items out of the system installed voices. The voices are then displayed in a listbox.
/// This allows the user to change the voice of the synthesizer in your app based on their preference.
/// </summary>
void Scenario_SynthesizeTextBoundaries::InitializeListboxVoiceChooser()
{
    // Get all of the installed voices.
    IVectorView<VoiceInformation^>^ voices = SpeechSynthesizer::AllVoices;

    // Get the currently selected voice.
    VoiceInformation^ currentVoice = synthesizer->Voice;

    // Copy the read-only voice information IVectorView into a mutable std::vector
    std::vector<VoiceInformation^> sortedVoices(voices->Size);
    std::copy(begin(voices), end(voices), sortedVoices.begin());

    // Sort it based on the language tag (en-US, fr-FR, etc), to group them for readability.
    std::sort(begin(sortedVoices), end(sortedVoices), [](VoiceInformation^ a, VoiceInformation^ b)
    {
        return b->Language > a->Language;
    });

    // Add them to the combo box in order.
    std::for_each(begin(sortedVoices), end(sortedVoices), [&](VoiceInformation^ voice)
    {
        ComboBoxItem^ item = ref new ComboBoxItem();
        item->Name = voice->DisplayName;
        item->Tag = voice;
        item->Content = voice->DisplayName + L" (" + voice->Language + L")";
        listboxVoiceChooser->Items->Append(item);

        // Check to see if we're looking at the current voice and set it as selected in the listbox.
        if (currentVoice->Id == voice->Id)
        {
            item->IsSelected = true;
            listboxVoiceChooser->SelectedItem = item;
        }
    });
}

/// <summary>
/// Register for all boundary events and register a function to add any new events if they arise.
/// </summary>
/// <param name="mediaPlaybackItem">The Media Playback Item to register events for.</param>
void Scenario_SynthesizeTextBoundaries::RegisterForBoundaryEvents(Windows::Media::Playback::MediaPlaybackItem^ mediaPlaybackItem)
{
    // If tracks were available at source resolution time, itterate through and register: 
    for (unsigned int index = 0; index < mediaPlaybackItem->TimedMetadataTracks->Size; index++)
    {
        RegisterMetadataHandlerForWords(mediaPlaybackItem, index);
        RegisterMetadataHandlerForSentences(mediaPlaybackItem, index);
    }

    // Since the tracks are added later we will  
    // monitor the tracks being added and subscribe to the ones of interest 
    mediaPlaybackItem->TimedMetadataTracksChanged += 
        ref new TypedEventHandler<MediaPlaybackItem^, IVectorChangedEventArgs^>
        (this, &Scenario_SynthesizeTextBoundaries::timedMetadataTrackChangedHandler);
}

/// <summary>
/// Register for boundary events when they arise.
/// </summary>
/// <param name="mediaPlaybackItem">The Media PLayback Item add handlers to.</param>
/// <param name="args">Arguments for the event.</param>
void Scenario_SynthesizeTextBoundaries::timedMetadataTrackChangedHandler(Windows::Media::Playback::MediaPlaybackItem^ mediaPlaybackItem,
    Windows::Foundation::Collections::IVectorChangedEventArgs^ args)
{
    if (args->CollectionChange == CollectionChange::ItemInserted)
    {
        RegisterMetadataHandlerForWords(mediaPlaybackItem, args->Index);
        RegisterMetadataHandlerForSentences(mediaPlaybackItem, args->Index);
    }
    else if (args->CollectionChange == CollectionChange::Reset)
    {
        for (unsigned int index = 0; index < mediaPlaybackItem->TimedMetadataTracks->Size; index++)
        {
            RegisterMetadataHandlerForWords(mediaPlaybackItem, index);
            RegisterMetadataHandlerForSentences(mediaPlaybackItem, index);
        }
    }
}


/// <summary>
/// Register for just word boundary events.
/// </summary>
/// <param name="mediaPlaybackItem">The Media PLayback Item add handlers to.</param>
/// <param name="index">Index of the timedMetadataTrack within the mediaPlaybackItem.</param>
void Scenario_SynthesizeTextBoundaries::RegisterMetadataHandlerForWords(Windows::Media::Playback::MediaPlaybackItem^ mediaPlaybackItem, int index)
{
    auto timedTrack = mediaPlaybackItem->TimedMetadataTracks->GetAt(index);
    //register for only word cues
    if (timedTrack->Id == "SpeechWord" && checkBoxWordBoundaries->IsChecked->Value == true)
    {
        timedTrack->CueEntered += ref new  TypedEventHandler<TimedMetadataTrack^, MediaCueEventArgs^>
            (this, &Scenario_SynthesizeTextBoundaries::metadata_SpeechCueEntered);
        mediaPlaybackItem->TimedMetadataTracks->SetPresentationMode(index, TimedMetadataTrackPresentationMode::ApplicationPresented);
    }
}

/// <summary>
/// Register for just sentence boundary events.
/// </summary>
/// <param name="mediaPlaybackItem">The Media Playback Item to register handlers for.</param>
/// <param name="index">Index of the timedMetadataTrack within the mediaPlaybackItem.</param>
void Scenario_SynthesizeTextBoundaries::RegisterMetadataHandlerForSentences(Windows::Media::Playback::MediaPlaybackItem^ mediaPlaybackItem, int index)
{
    auto timedTrack = mediaPlaybackItem->TimedMetadataTracks->GetAt(index);
    //register for only sentence cues
    if (timedTrack->Id == "SpeechSentence" && (bool)checkBoxSentenceBoundaries->IsChecked->Value == true)
    {
        timedTrack->CueEntered += ref new  TypedEventHandler<TimedMetadataTrack^, MediaCueEventArgs^>
            (this, &Scenario_SynthesizeTextBoundaries::metadata_SpeechCueEntered);
        mediaPlaybackItem->TimedMetadataTracks->SetPresentationMode(index, TimedMetadataTrackPresentationMode::ApplicationPresented);
    }
}

/// <summary>
/// This function executes when a SpeechCue is hit and calls the functions to update the UI
/// </summary>
/// <param name="timedMetadataTrack">The timedMetadataTrack associated with the event.</param>
/// <param name="args">the arguments associated with the event.</param>
void Scenario_SynthesizeTextBoundaries::metadata_SpeechCueEntered(Windows::Media::Core::TimedMetadataTrack^ timedMetadataTrack, 
                                                                  Windows::Media::Core::MediaCueEventArgs^ args)
{
    // Check in case there are different tracks and the handler was used for more tracks 
    if (timedMetadataTrack->TimedMetadataKind == TimedMetadataKind::Speech)
    {
        auto cue = (SpeechCue^)args->Cue;
        if (cue != nullptr)
        {
            printf("Hit Cue with start:%d and end: %d\r\n", cue->StartPositionInInput->Value,cue->EndPositionInInput->Value);
            wprintf(L"Cue text:[%s]\r\n", cue->Text->Data());
            
            // Do something with the cue 
            // Because this call is not awaited, execution of the current method continues before the call is completed
            Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal,
                ref new Windows::UI::Core::DispatchedHandler([this, cue, timedMetadataTrack]()
            {
                // Your UI update code goes here!
                HighlightWordOnScreen(cue->StartPositionInInput->Value, cue->EndPositionInInput->Value);
                FillTextBoxes(cue, timedMetadataTrack);
            }));
        }
    }
}

/// <summary>
/// This function selects the word associated with the start and end positions.
/// </summary>
/// <param name="startPositionInput"> the starting index of the word </param>
/// <param name="endPositionInInput"> the ending index of the word </param>
void Scenario_SynthesizeTextBoundaries::HighlightWordOnScreen(int startPositionInInput, int endPositionInInput)
{
    //since we are using indecies we need to add 1 to the end position
    textToSynthesize->Select((int)startPositionInInput, (int)(endPositionInInput + 1) - (int)startPositionInInput);
}

/// <summary>
/// This function executes when a SpeechCue is hit and calls the functions to update the UI
/// </summary>
/// <param name="timedMetadataTrack">The timedMetadataTrack associated with the event.</param>
/// <param name="cue">the SpeechCue object.</param>
void Scenario_SynthesizeTextBoundaries::FillTextBoxes(Windows::Media::Core::SpeechCue ^ cue, Windows::Media::Core::TimedMetadataTrack ^ timedMetadataTrack)
{
    //if it is a sentence cue, populate the sentence text box.
    if (timedMetadataTrack->Id == "SpeechSentence")
    {
        textBoxLastSpeechSentence->Text = cue->Text;
    }
    //if it is a word cue, populate the word text box
    if (timedMetadataTrack->Id == "SpeechWord")
    {
        textBoxLastSpeechWord->Text = cue->Text;
    }
}
