//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using SDKTemplate;
using System;
using Windows.Media.SpeechSynthesis;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SpeechAndTTS
{
    public sealed partial class SynthesizeTextScenario : Page
    {
        private MainPage rootPage;
        private SpeechSynthesizer synthesizer;

        public SynthesizeTextScenario()
        {
            InitializeComponent();
            synthesizer = new SpeechSynthesizer();
            ListboxVoiceChooser_Initialize();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        /// <summary>
        /// This is invoked when the user clicks on the speak/stop button.
        /// </summary>
        /// <param name="sender">unused object parameter</param>
        /// <param name="e">unused event parameter</param>
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

                // Change the button label. You could also just disable the button if you don't want any user control.
                btnSpeak.Content = "Stop";

                SpeechSynthesisStream synthesisStream;
                try
                {
                    // Create a stream from the text. This will be played using a media element.
                    synthesisStream = await synthesizer.SynthesizeTextToStreamAsync(text);

					// Set the source and start playing the synthesized audio stream.
					media.AutoPlay = true;
					media.SetSource(synthesisStream, synthesisStream.ContentType);
					media.Play();
				}
                catch (Exception)
                {
					// If the SSML stream is not in the correct format, throw an error message to the user.
					btnSpeak.Content = "Speak";
                    MessageDialog dialog = new MessageDialog("Unable to synthesize text");
                    await dialog.ShowAsync();
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
        private void ListboxVoiceChooser_Initialize()
        {
            // Get all of the installed voices.
            var voices = SpeechSynthesizer.AllVoices;

            // Get the currently selected voice.
            VoiceInformation currentVoice = synthesizer.Voice;

            foreach (VoiceInformation voice in voices)
            {
                ComboBoxItem item = new ComboBoxItem();
                item.Name = voice.DisplayName;
                item.Tag = voice;
                item.Content = voice.DisplayName;
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
        }
    }
}
