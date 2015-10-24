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

using namespace SDKTemplate;
using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Resources::Core;
using namespace Windows::Data::Xml::Dom;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
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
}

/// <summary>
/// This is invoked when the user clicks on the speak/stop button.
/// </summary>
/// <param name="sender">Button that triggered this event</param>
/// <param name="e">State information about the routed event</param>
void Scenario_SynthesizeSSML::Speak_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // If the media is playing, the user has pressed the button to stop the playback.
    if (media->CurrentState == MediaElementState::Playing)
    {
        media->Stop();
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
            create_task(synthesizer->SynthesizeSsmlToStreamAsync(text), task_continuation_context::use_current())
                .then([this](task<SpeechSynthesisStream^> synthesisStreamTask)
            {
                try
                {
                    SpeechSynthesisStream^ synthesisStream = synthesisStreamTask.get();
                    // Set the source and start playing the synthesized audio stream.
                    media->AutoPlay = true;
                    media->SetSource(synthesisStream, synthesisStream->ContentType);
                    media->Play();
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
						// Typically occurs if the XML provided isn't valid.
						btnSpeak->Content = "Speak";
						Windows::UI::Popups::MessageDialog^ dialog = ref new Windows::UI::Popups::MessageDialog(ex->Message, "Unable to synthesize text (check for invalid xml)");
						create_task(dialog->ShowAsync());
					}
                }
            });
        }
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
void Scenario_SynthesizeSSML::media_MediaEnded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    btnSpeak->Content = "Speak";
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