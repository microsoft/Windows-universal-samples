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

using System;
using System.Linq;
using Windows.ApplicationModel.Resources.Core;
using Windows.Foundation.Collections;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.Media.SpeechSynthesis;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario_SynthesizeTextBoundaries : Page
    {
        private SpeechSynthesizer synthesizer;
        private ResourceContext speechContext;
        private ResourceMap speechResourceMap;

        public Scenario_SynthesizeTextBoundaries()
        {
            InitializeComponent();
            synthesizer = new SpeechSynthesizer();

            speechContext = ResourceContext.GetForCurrentView();
            speechContext.Languages = new string[] { SpeechSynthesizer.DefaultVoice.Language };

            speechResourceMap = ResourceManager.Current.MainResourceMap.GetSubtree("LocalizationTTSResources");

            MediaPlayer player = new MediaPlayer();
            player.AutoPlay = false;
            player.MediaEnded += media_MediaEnded;

            media.SetMediaPlayer(player);

            InitializeListboxVoiceChooser();
        }

        /// <summary>
        /// This is invoked when the user clicks on the speak/stop button.
        /// </summary>
        /// <param name="sender">Button that triggered this event</param>
        /// <param name="e">State information about the routed event</param>
        private async void Speak_Click(object sender, RoutedEventArgs e)
        {
            // If the media is playing, the user has pressed the button to stop the playback.
            if (media.MediaPlayer.PlaybackSession.PlaybackState == MediaPlaybackState.Playing)
            {
                media.MediaPlayer.Pause();
                btnSpeak.Content = "Speak";
            }
            else
            {
                string text = textToSynthesize.Text;
                if (!String.IsNullOrEmpty(text))
                {
                    // Change the button label. You could also just disable the button if you don't want any user control.
                    btnSpeak.Content = "Stop";

                    try
                    {
                        // Enable word marker generation (false by default). 
                        synthesizer.Options.IncludeWordBoundaryMetadata = true;
                        synthesizer.Options.IncludeSentenceBoundaryMetadata = true;

                        SpeechSynthesisStream synthesisStream = await synthesizer.SynthesizeTextToStreamAsync(text);

                        // Create a media source from the stream: 
                        var mediaSource = MediaSource.CreateFromStream(synthesisStream, synthesisStream.ContentType);

                        //Create a Media Playback Item   
                        var mediaPlaybackItem = new MediaPlaybackItem(mediaSource);

                        // Ensure that the app is notified for cues.  
                        RegisterForWordBoundaryEvents(mediaPlaybackItem);

                        // Set the source of the MediaElement or MediaPlayerElement to the MediaPlaybackItem 
                        // and start playing the synthesized audio stream.
                        media.Source = mediaPlaybackItem;
                        media.MediaPlayer.Play();
                    }
                    catch (System.IO.FileNotFoundException)
                    {
                        // If media player components are unavailable, (eg, using a N SKU of windows), we won't
                        // be able to start media playback. Handle this gracefully
                        btnSpeak.Content = "Speak";
                        btnSpeak.IsEnabled = false;
                        textToSynthesize.IsEnabled = false;
                        listboxVoiceChooser.IsEnabled = false;
                        var messageDialog = new Windows.UI.Popups.MessageDialog("Media player components unavailable");
                        await messageDialog.ShowAsync();
                    }
                    catch (Exception)
                    {
                        // If the text is unable to be synthesized, throw an error message to the user.
                        btnSpeak.Content = "Speak";
                        media.AutoPlay = false;
                        var messageDialog = new Windows.UI.Popups.MessageDialog("Unable to synthesize text");
                        await messageDialog.ShowAsync();
                    }
                }
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
        async void media_MediaEnded(MediaPlayer sender, object e)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal,
             () =>
             {
                 // Your UI update code goes here!
                 btnSpeak.Content = "Speak";
             });
        }

        /// <summary>
        /// This creates items out of the system installed voices. The voices are then displayed in a listbox.
        /// This allows the user to change the voice of the synthesizer in your app based on their preference.
        /// </summary>
        private void InitializeListboxVoiceChooser()
        {
            // Get all of the installed voices.
            var voices = SpeechSynthesizer.AllVoices;

            // Get the currently selected voice.
            VoiceInformation currentVoice = synthesizer.Voice;

            foreach (VoiceInformation voice in voices.OrderBy(p => p.Language))
            {
                ComboBoxItem item = new ComboBoxItem();
                item.Name = voice.DisplayName;
                item.Tag = voice;
                item.Content = voice.DisplayName + " (Language: " + voice.Language + ")";
                listboxVoiceChooser.Items.Add(item);

                // Check to see if we're looking at the current voice and set it as selected in the listbox.
                if (currentVoice.Id == voice.Id)
                {
                    item.IsSelected = true;
                    listboxVoiceChooser.SelectedItem = item;
                }
            }
        }

        /// <summary>
        /// This is called when the user has selects a voice from the drop down.
        /// </summary>
        /// <param name="sender">unused object parameter</param>
        /// <param name="e">unused event parameter</param>
        private void ListboxVoiceChooser_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBoxItem item = (ComboBoxItem)(listboxVoiceChooser.SelectedItem);
            VoiceInformation voice = (VoiceInformation)(item.Tag);
            synthesizer.Voice = voice;

            // update UI text to be an appropriate default translation.
            speechContext.Languages = new string[] { voice.Language };
            textToSynthesize.Text = speechResourceMap.GetValue("SynthesizeTextBoundariesDefaultText", speechContext).ValueAsString;
        }

        /// <summary>
        /// This function executes when a SpeechCue is hit and calls the functions to update the UI
        /// </summary>
        /// <param name="timedMetadataTrack">The timedMetadataTrack associated with the event.</param>
        /// <param name="args">the arguments associated with the event.</param>
        private async void metadata_SpeechCueEntered(TimedMetadataTrack timedMetadataTrack, MediaCueEventArgs args)
        {
            // Check in case there are different tracks and the handler was used for more tracks 
            if (timedMetadataTrack.TimedMetadataKind == TimedMetadataKind.Speech)
            {
                var cue = args.Cue as SpeechCue;
                if (cue != null)
                {
                    System.Diagnostics.Debug.WriteLine("Hit Cue with start:" + cue.StartPositionInInput + " and end:" + cue.EndPositionInInput);
                    System.Diagnostics.Debug.WriteLine("Cue text:[" + cue.Text + "]");
                    // Do something with the cue 
                    await Dispatcher.RunAsync(CoreDispatcherPriority.Normal,
                     () =>
                     {
                         // Your UI update code goes here!
                         HighlightTextOnScreen(cue.StartPositionInInput, cue.EndPositionInInput);
                         FillTextBoxes(cue, timedMetadataTrack);
                     });
                }
            }
        }

        /// <summary>
        /// This function executes when a SpeechCue is hit and calls the functions to update the UI
        /// </summary>
        /// <param name="timedMetadataTrack">The timedMetadataTrack associated with the event.</param>
        /// <param name="cue">the SpeechCue object.</param>
        private void FillTextBoxes(SpeechCue cue, TimedMetadataTrack timedMetadataTrack)
        {
            //if it is a sentence cue, populate the sentence text box.
            if(timedMetadataTrack.Id == "SpeechSentence")
            {
                textBoxLastSpeechSentence.Text = cue.Text;
            }
            //if it is a word cue, populate the word text box
            if(timedMetadataTrack.Id == "SpeechWord")
            {
                textBoxLastSpeechWord.Text = cue.Text;
            }
        }

        /// <summary>
        /// This function selects the text associated with the start and end positions.
        /// </summary>
        /// <param name="startPositionInput"> the starting index of the word </param>
        /// <param name="endPositionInInput"> the ending index of the word </param>
        private void HighlightTextOnScreen(int? startPositionInInput, int? endPositionInInput)
        {
            if (startPositionInInput != null && endPositionInInput != null)
            {
                textToSynthesize.Select(startPositionInInput.Value, endPositionInInput.Value - startPositionInInput.Value + 1);
            }
        }

        /// <summary>
        /// Register for all boundary events and register a function to add any new events if they arise.
        /// </summary>
        /// <param name="mediaPlaybackItem">The Media PLayback Item add handlers to.</param>
        private void RegisterForWordBoundaryEvents(MediaPlaybackItem mediaPlaybackItem)
        {
            // If tracks were available at source resolution time, itterate through and register: 
            for (int index = 0; index < mediaPlaybackItem.TimedMetadataTracks.Count; index++)
            {
                RegisterMetadataHandlerForWords(mediaPlaybackItem, index);
                RegisterMetadataHandlerForSentences(mediaPlaybackItem, index);
            }

            // Since the tracks are added later we will  
            // monitor the tracks being added and subscribe to the ones of interest 
            mediaPlaybackItem.TimedMetadataTracksChanged += (MediaPlaybackItem sender, IVectorChangedEventArgs args) =>
            {
                if (args.CollectionChange == CollectionChange.ItemInserted)
                {
                    RegisterMetadataHandlerForWords(sender, (int)args.Index);
                    RegisterMetadataHandlerForSentences(mediaPlaybackItem, (int)args.Index);
                }
                else if (args.CollectionChange == CollectionChange.Reset)
                {
                    for (int index = 0; index < sender.TimedMetadataTracks.Count; index++)
                    {
                        RegisterMetadataHandlerForWords(sender, index);
                        RegisterMetadataHandlerForSentences(mediaPlaybackItem, index);
                    }
                }
            };
        }

        /// <summary>
        /// Register for just word boundary events.
        /// </summary>
        /// <param name="mediaPlaybackItem">The Media Playback Item to register handlers for.</param>
        /// <param name="index">Index of the timedMetadataTrack within the mediaPlaybackItem.</param>
        private void RegisterMetadataHandlerForWords(MediaPlaybackItem mediaPlaybackItem, int index)
        {
            var timedTrack = mediaPlaybackItem.TimedMetadataTracks[index];
            //register for only word cues
            if (timedTrack.Id == "SpeechWord" && checkBoxWordBoundaries.IsChecked.Value)
            {
                timedTrack.CueEntered += metadata_SpeechCueEntered;
                mediaPlaybackItem.TimedMetadataTracks.SetPresentationMode((uint)index, TimedMetadataTrackPresentationMode.ApplicationPresented);
            }
        }

        /// <summary>
        /// Register for just sentence boundary events.
        /// </summary>
        /// <param name="mediaPlaybackItem">The Media Playback Item to register handlers for.</param>
        /// <param name="index">Index of the timedMetadataTrack within the mediaPlaybackItem.</param>
        private void RegisterMetadataHandlerForSentences(MediaPlaybackItem mediaPlaybackItem, int index)
        {
            var timedTrack = mediaPlaybackItem.TimedMetadataTracks[index];
            //register for only sentence cues
            if (timedTrack.Id == "SpeechSentence" && checkBoxSentenceBoundaries.IsChecked.Value)
            {
                timedTrack.CueEntered += metadata_SpeechCueEntered;
                mediaPlaybackItem.TimedMetadataTracks.SetPresentationMode((uint)index, TimedMetadataTrackPresentationMode.ApplicationPresented);
            }
        }
    }
}
