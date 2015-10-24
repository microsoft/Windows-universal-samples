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
using Windows.Globalization;
using Windows.Media.SpeechSynthesis;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace SpeechAndTTS
{
    public sealed partial class SynthesizeTextScenario : Page
    {
        private SpeechSynthesizer synthesizer;
        private ResourceContext speechContext;
        private ResourceMap speechResourceMap;

        public SynthesizeTextScenario()
        {
            InitializeComponent();
            synthesizer = new SpeechSynthesizer();
            
            speechContext = ResourceContext.GetForCurrentView();
            speechContext.Languages = new string[] { SpeechSynthesizer.DefaultVoice.Language };

            speechResourceMap = ResourceManager.Current.MainResourceMap.GetSubtree("LocalizationTTSResources");

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
            if (media.CurrentState.Equals(MediaElementState.Playing))
            {
                media.Stop();
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
                        // Create a stream from the text. This will be played using a media element.
                        SpeechSynthesisStream synthesisStream = await synthesizer.SynthesizeTextToStreamAsync(text);

                        // Set the source and start playing the synthesized audio stream.
                        media.AutoPlay = true;
                        media.SetSource(synthesisStream, synthesisStream.ContentType);
                        media.Play();
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
        void media_MediaEnded(object sender, RoutedEventArgs e)
        {
            btnSpeak.Content = "Speak";
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

            foreach (VoiceInformation voice in voices.OrderBy(p=>p.Language))
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
            textToSynthesize.Text = speechResourceMap.GetValue("SynthesizeTextDefaultText", speechContext).ValueAsString;
        }
    }
}
