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
using Windows.Data.Xml.Dom;
using Windows.Foundation.Collections;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.Media.SpeechSynthesis;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace SDKTemplate
{
    public sealed partial class Scenario_SynthesizeSSML : Page
    {
        private SpeechSynthesizer synthesizer;
        private ResourceContext speechContext;
        private ResourceMap speechResourceMap;

        public Scenario_SynthesizeSSML()
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
            media.MediaPlayer.AutoPlay = false;

            InitializeListboxVoiceChooser();
            UpdateSSMLText();
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
                if (!string.IsNullOrEmpty(text))
                {
                    // Change the button label. You could also just disable the button if you don't want any user control.
                    btnSpeak.Content = "Stop";

                    try
                    {
                        // Create a stream from the text. This will be played using a media element.
                        SpeechSynthesisStream synthesisStream = await synthesizer.SynthesizeSsmlToStreamAsync(text);

                        // Create a media source from the stream: 
                        var mediaSource = MediaSource.CreateFromStream(synthesisStream, synthesisStream.ContentType);

                        //Create a Media Playback Item   
                        var mediaPlaybackItem = new MediaPlaybackItem(mediaSource);

                        // Ensure that the app is notified for marks.  
                        RegisterForMarkEvents(mediaPlaybackItem);

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
                        // If the SSML stream is not in the correct format, throw an error message to the user.
                        btnSpeak.Content = "Speak";
                        var messageDialog = new Windows.UI.Popups.MessageDialog("Unable to synthesize text");
                        await messageDialog.ShowAsync();
                    }
                }
            }
        }

        /// <summary>
        /// Register for all mark events
        /// </summary>
        /// <param name="mediaPlaybackItem">The Media PLayback Item add handlers to.</param>
        private void RegisterForMarkEvents(MediaPlaybackItem mediaPlaybackItem)
        {
            //tracks could all be generated at creation
            for (int index =0; index < mediaPlaybackItem.TimedMetadataTracks.Count; index++)
            {
                RegisterMetadataHandlerForMarks(mediaPlaybackItem, index);
            }

            // if the tracks are added later we will  
            // monitor the tracks being added and subscribe to the ones of interest 
            mediaPlaybackItem.TimedMetadataTracksChanged += (MediaPlaybackItem sender, IVectorChangedEventArgs args) =>
            {
                if (args.CollectionChange == CollectionChange.ItemInserted)
                {
                    RegisterMetadataHandlerForMarks(sender, (int)args.Index);
                }
                else if (args.CollectionChange == CollectionChange.Reset)
                {
                    for (int index = 0; index < sender.TimedMetadataTracks.Count; index++)
                    {
                        RegisterMetadataHandlerForMarks(sender, index);
                    }
                }
            };
        }

        /// <summary>
        /// Register for just word bookmark events.
        /// </summary>
        /// <param name="mediaPlaybackItem">The Media Playback Item to register handlers for.</param>
        /// <param name="index">Index of the timedMetadataTrack within the mediaPlaybackItem.</param>
        private void RegisterMetadataHandlerForMarks(MediaPlaybackItem mediaPlaybackItem, int index)
        {
            //make sure we only register for bookmarks
            var timedTrack = mediaPlaybackItem.TimedMetadataTracks[index];
            if(timedTrack.Id == "SpeechBookmark")
            {
                timedTrack.CueEntered += metadata_MarkCueEntered;
                mediaPlaybackItem.TimedMetadataTracks.SetPresentationMode((uint)index, TimedMetadataTrackPresentationMode.ApplicationPresented);

            }
        }

        /// <summary>
        /// This function executes when a SpeechCue is hit and calls the functions to update the UI
        /// </summary>
        /// <param name="timedMetadataTrack">The timedMetadataTrack associated with the event.</param>
        /// <param name="args">the arguments associated with the event.</param>
        private async void metadata_MarkCueEntered(TimedMetadataTrack timedMetadataTrack, MediaCueEventArgs args)
        {
            // Check in case there are different tracks and the handler was used for more tracks 
            if (timedMetadataTrack.TimedMetadataKind == TimedMetadataKind.Speech)
            {
                var cue = args.Cue as SpeechCue;
                if (cue != null)
                {
                    System.Diagnostics.Debug.WriteLine("Cue text:[" + cue.Text + "]");
                    // Do something with the cue 
                    await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal,
                     () =>
                     {
                         // Your UI update code goes here!
                         FillTextBox(cue);
                     });
                }
            }
        }

        /// <summary>
        /// Update the UI with text from the mark
        /// </summary>
        /// <param name="cue">The cue containing the text</param>
        private void FillTextBox(SpeechCue cue)
        {
            textBoxLastMarkTriggered.Text = cue.Text;
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
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal,
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
            textToSynthesize.Text = speechResourceMap.GetValue("SynthesizeSSMLDefaultText", speechContext).ValueAsString;
            
            UpdateSSMLText();
        }

        /// <summary>
        /// Update the text in the SSML field to match the language of the chosen synthesizer voice.
        /// </summary>
        /// <remarks>
        /// SSML language takes priority over the chosen synthesizer language.
        /// Thus, when changing voices, we need to update the SSML language to match.
        /// </remarks>
        private void UpdateSSMLText()
        {
            try
            {
                string text = textToSynthesize.Text;
                string language = synthesizer.Voice.Language;

                XmlDocument doc = new XmlDocument();
                doc.LoadXml(text);

                var LangAttribute = doc.DocumentElement.GetAttributeNode("xml:lang");
                LangAttribute.InnerText = language;

                textToSynthesize.Text = doc.GetXml();
            }
            catch
            {
                // This can fail if the user is in the process of editing the XML.
                // In this case, we don't update the SSML language but also don't throw a failure.
            }
        }
    }
}
