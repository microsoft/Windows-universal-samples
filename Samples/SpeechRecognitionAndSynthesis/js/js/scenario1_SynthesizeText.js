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

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario1_SynthesizeText.html", {
        ready: function (element, options) {
            try {
                synthesizer = new Windows.Media.SpeechSynthesis.SpeechSynthesizer();
                audio = new Audio();

                btnSpeak.addEventListener("click", speakFn, false);
                voicesSelect.addEventListener("click", setVoiceFunction, false);

                var rcns = Windows.ApplicationModel.Resources.Core;
                context = new rcns.ResourceContext();
                context.languages = new Array(synthesizer.voice.language);
                resourceMap = rcns.ResourceManager.current.mainResourceMap.getSubtree('LocalizationTTSResources');

                textToSynthesize.innerText = resourceMap.getValue('SynthesizeTextDefaultText', context).valueAsString;

                listbox_GetVoices();
                audio_SetUp();
            } catch (exception) {
                if (exception.number == -2147467263) {// E_NOTIMPL

                    // If media player components aren't installed (for example, when using an N SKU of windows)
                    // this error may occur when instantiating the Audio object.
                    statusMessage.innerText = "Media Player components are not available.";
                    statusBox.style.backgroundColor = "red";
                    btnSpeak.disabled = true;
                    textToSynthesize.disabled = true;
                }
            }
        },

        unload: function (element, options) {
            if (audio != null) {
                audio.onpause = null;
                audio.pause();
            }
        }
    });

    var synthesizer;
    var audio;

    // localization resources
    var context;
    var resourceMap;

    function audio_SetUp() {
        /// <summary>
        /// Sets up the audio element's events so the app UI updates based on the current state of playback.
        /// </summary>
        audio.onplay = function () { // Fires when the audio begins playing
            statusMessage.innerText = "Playing";
        };

        audio.onpause = function () { // Fires when the user presses the stop button
            statusMessage.innerText = "Completed";
            btnSpeak.innerText = "Speak";
        };

        audio.onended = function () { // Fires when the audio finishes playing
            statusMessage.innerText = "Completed";
            btnSpeak.innerText = "Speak";
            voicesSelect.disabled = false;
        };
    }

    function speakFn() {
        /// <summary>
        /// This is invoked when the user clicks on the speak/stop button. It attempts to convert the 
        /// textbox content into a stream, then plays the stream through the audio element.
        /// </summary>
        if (btnSpeak.innerText == "Stop") {
            voicesSelect.disabled = false;
            audio.pause();
            return;
        }

        // Changes the button label. You could also just disable the button if you don't want any user control.
        voicesSelect.disabled = true;
        btnSpeak.innerText = "Stop";
        statusBox.style.backgroundColor = "green";

        // Creates a stream from the text. This will be played using an audio element.
        synthesizer.synthesizeTextToStreamAsync(textToSynthesize.value).done(
            function (markersStream) {
                // Set the source and start playing the synthesized audio stream.
                var blob = MSApp.createBlobFromRandomAccessStream(markersStream.ContentType, markersStream);
                audio.src = URL.createObjectURL(blob, { oneTimeOnly: true });
                markersStream.seek(0);
                audio.play();
            },
            function (error) {
                errorMessage(error.message);
            });
    }

    function listbox_GetVoices() {
        /// <summary>
        /// This creates items out of the system installed voices. The voices are then displayed in a listbox.
        /// This allows the user to change the voice of the synthesizer in your app based on their preference.
        /// </summary>

        // Get the list of all of the voices installed on this machine.
        var allVoices = Windows.Media.SpeechSynthesis.SpeechSynthesizer.allVoices;

        // Get the currently selected voice.
        var defaultVoice = Windows.Media.SpeechSynthesis.SpeechSynthesizer.defaultVoice;

        for (var voiceIndex = 0; voiceIndex < allVoices.size; voiceIndex++) {
            var currVoice = allVoices[voiceIndex];
            var option = document.createElement("option");
            option.text = currVoice.displayName + " (" + currVoice.language + ")";
            voicesSelect.add(option, null);

            // Check to see if we're looking at the current voice and set it as selected in the listbox.
            if (currVoice.id === defaultVoice.id) {
                voicesSelect.selectedIndex = voiceIndex;
            }
        }
    }

    function setVoiceFunction() {
        /// <summary>
        /// This is called when the user selects a voice from the drop down.
        /// </summary>
        if (voicesSelect.selectedIndex !== -1) {
            var allVoices = Windows.Media.SpeechSynthesis.SpeechSynthesizer.allVoices;

            // Use the selected index to find the voice.
            var selectedVoice = allVoices[voicesSelect.selectedIndex];

            synthesizer.voice = selectedVoice;

            // change the language of the sample text.
            context.languages = new Array(synthesizer.voice.language);
            textToSynthesize.innerText = resourceMap.getValue('SynthesizeTextDefaultText', context).valueAsString;
        }
    }

    function errorMessage(text) {
        /// <summary>
        /// Sets the specified text area with the error message details.
        /// </summary>
        errorTextArea.innerText = text;
    }
})();
