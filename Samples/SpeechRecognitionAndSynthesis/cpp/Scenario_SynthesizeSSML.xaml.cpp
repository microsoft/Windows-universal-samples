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
#include "Scenario_SynthesizeSSML.xaml.h"
#include <pplawait.h>

using namespace SDKTemplate;
using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Resources::Core;
using namespace Windows::Data::Xml::Dom;
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

Scenario_SynthesizeSSML::Scenario_SynthesizeSSML() : rootPage(MainPage::Current)
{
    InitializeComponent();
    synthesizer = ref new SpeechSynthesizer();

    // initialize localization for SSML text blob
    speechContext = ResourceContext::GetForCurrentView();
    speechContext->Languages = ref new VectorView<String^>(1, SpeechSynthesizer::DefaultVoice->Language);

    speechResourceMap = ResourceManager::Current->MainResourceMap->GetSubtree(L"LocalizationTTSResources");

    InitializeListboxVoiceChooser();
    UpdateSSMLText();

    MediaPlayer^ player = ref new MediaPlayer();
    player->AutoPlay = false;
    player->MediaEnded += ref new  TypedEventHandler<MediaPlayer^, Object^>(this, &Scenario_SynthesizeSSML::media_MediaEnded);

    media->SetMediaPlayer(player);
}

/// <summary>
/// This is invoked when the user clicks on the speak/stop button.
/// </summary>
/// <param name="sender">Button that triggered this event</param>
/// <param name="e">State information about the routed event</param>
void Scenario_SynthesizeSSML::Speak_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
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

            // Create a stream from the text. This will be played using a media element.
            SynthesizeSSMLStream();
        }
    }
}

task<void> Scenario_SynthesizeSSML::SynthesizeSSMLStream()
{
    String^ text = textToSynthesize->Text;

    return create_task(synthesizer->SynthesizeSsmlToStreamAsync(text))
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
            RegisterForMarkEvents(mediaPlaybackItem);

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
                Windows::UI::Popups::MessageDialog^ dialog = ref new Windows::UI::Popups::MessageDialog(ex->Message, "Media playback components unavailable.");
                create_task(dialog->ShowAsync());
            }
            else
            {
                // If the text is not able to be synthesized, throw an error message to the user.
                btnSpeak->Content = L"Speak";
                Windows::UI::Popups::MessageDialog^ dialog = ref new Windows::UI::Popups::MessageDialog(ex->Message, "Unable to synthesize text");
                create_task(dialog->ShowAsync());
            }
        }
    });
}

/// <summary>
/// Register for all mark events
/// </summary>
/// <param name="mediaPlaybackItem">The Media PLayback Item add handlers to.</param>
void Scenario_SynthesizeSSML::RegisterForMarkEvents(Windows::Media::Playback::MediaPlaybackItem^ mediaPlaybackItem)
{
    // If tracks were available at source resolution time, itterate through and register: 
    for (unsigned int index = 0; index < mediaPlaybackItem->TimedMetadataTracks->Size; index++)
    {
        RegisterMetadataHandlerForMarks(mediaPlaybackItem, index);
    }
}

/// <summary>
/// Register for just word bookmark events.
/// </summary>
/// <param name="mediaPlaybackItem">The Media Playback Item to register handlers for.</param>
/// <param name="index">Index of the timedMetadataTrack within the mediaPlaybackItem.</param>
void Scenario_SynthesizeSSML::RegisterMetadataHandlerForMarks(Windows::Media::Playback::MediaPlaybackItem^ mediaPlaybackItem, int index)
{
    auto timedTrack = mediaPlaybackItem->TimedMetadataTracks->GetAt(index);
    //register for only mark cues
    if (timedTrack->Id == "SpeechBookmark" )
    {
        timedTrack->CueEntered += ref new  TypedEventHandler<TimedMetadataTrack^, MediaCueEventArgs^>(this, &Scenario_SynthesizeSSML::metadata_MarkCueEntered);
        mediaPlaybackItem->TimedMetadataTracks->SetPresentationMode(index, TimedMetadataTrackPresentationMode::ApplicationPresented);
    }
}

/// <summary>
/// This is called when the user has selects a voice from the drop down.
/// </summary>
/// <param name="sender">unused object parameter</param>
/// <param name="e">unused event parameter</param>
void Scenario_SynthesizeSSML::ListboxVoiceChooser_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
    if (e->AddedItems->Size > 0)
    {
        ComboBoxItem^ first = safe_cast<ComboBoxItem^>(e->AddedItems->GetAt(0));
        VoiceInformation^ voice = safe_cast<VoiceInformation^>(first->Tag);
        synthesizer->Voice = voice;

        // update UI text to be an appropriate default translation.
        speechContext->Languages = ref new VectorView<String^>(1, { voice->Language });
        textToSynthesize->Text = speechResourceMap->GetValue("SynthesizeSSMLDefaultText", speechContext)->ValueAsString;

        UpdateSSMLText();

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
void Scenario_SynthesizeSSML::media_MediaEnded(Windows::Media::Playback::MediaPlayer^ timedMetadataTrack, Platform::Object^ e)
{
    // Because this call is not awaited, execution of the current method continues before the call is completed
    Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal,
        ref new Windows::UI::Core::DispatchedHandler([this]()
    {
        btnSpeak->Content = "Speak";
    }));
}

/// <summary>
/// This function executes when a SpeechCue is hit and calls the functions to update the UI
/// </summary>
/// <param name="timedMetadataTrack">The timedMetadataTrack associated with the event.</param>
/// <param name="args">the arguments associated with the event.</param>
void Scenario_SynthesizeSSML::metadata_MarkCueEntered(Windows::Media::Core::TimedMetadataTrack^ timedMetadataTrack, Windows::Media::Core::MediaCueEventArgs^ args)
{
    // Check in case there are different tracks and the handler was used for more tracks 
    if (timedMetadataTrack->TimedMetadataKind == TimedMetadataKind::Speech)
    {
        auto cue = (SpeechCue^)args->Cue;
        if (cue != nullptr)
        {
            wprintf(L"Cue text:[%s]\r\n", cue->Text->Data());

            // Do something with the cue 
            // Because this call is not awaited, execution of the current method continues before the call is completed
            Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal,
                ref new Windows::UI::Core::DispatchedHandler([this, cue, timedMetadataTrack]()
            {
                // Your UI update code goes here!
                FillTextBox(cue);
            }));
        }
    }
}

/// <summary>
/// Update the UI with text from the mark
/// </summary>
/// <param name="cue">The cue containing the text</param>
void Scenario_SynthesizeSSML::FillTextBox(SpeechCue^ cue)
{
    textBoxLastMarkTriggered->Text = cue->Text;
}

/// <summary>
/// This creates items out of the system installed voices. The voices are then displayed in a listbox.
/// This allows the user to change the voice of the synthesizer in your app based on their preference.
/// </summary>
void Scenario_SynthesizeSSML::InitializeListboxVoiceChooser()
{
    // Get all of the installed voices.
    IVectorView<VoiceInformation^>^ voices = SpeechSynthesizer::AllVoices;

    // Get the currently selected voice.
    VoiceInformation^ currentVoice = synthesizer->Voice;

    // Copy the read-only voice information IVectorView into a mutable std::vector
    std::vector<VoiceInformation^> sortedVoices(voices->Size);
    std::copy(begin(voices), end(voices), sortedVoices.begin());

    // Sort it based on the language tag (en-US, fr-FR, etc)
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
/// Update the text in the SSML field to match the language of the chosen synthesizer voice.
/// </summary>
/// <remarks>
/// SSML language takes priority over the chosen synthesizer language.
/// Thus, when changing voices, we need to update the SSML language to match.
/// </remarks>
void Scenario_SynthesizeSSML::UpdateSSMLText()
{
    try
    {
        String^ text = textToSynthesize->Text;
        String^ language = synthesizer->Voice->Language;

        XmlDocument^ doc = ref new XmlDocument();
        doc->LoadXml(text);

        auto LangAttribute = doc->DocumentElement->GetAttributeNode("xml:lang");
        LangAttribute->InnerText = language;

        textToSynthesize->Text = doc->GetXml();
    }
    catch (Exception^)
    {
        // This can fail if the user is in the process of editing the XML.
        // In this case, we don't update the SSML language but also don't throw a failure.
    }
}