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
    var page = WinJS.UI.Pages.define("/html/scenario2_SynthesizeTextBoundaries.html", {
        ready: function (element, options) {
            try {
                synthesizer = new Windows.Media.SpeechSynthesis.SpeechSynthesizer();
                audio = new Audio();

                // 
                // Create a track on the HTML5 audio tag to receive the metadata created  
                // by the speech synthesizer 
                // 
                marksMetadataTrackForAudio = audio.addTextTrack("metadata", "SpeechBoundaries");
                marksMetadataTrackForAudio.addEventListener("cuechange", cueChange, false);

                btnSpeak.addEventListener("click", speakFn, false);
                voicesSelect.addEventListener("click", setVoiceFunction, false);

                var rcns = Windows.ApplicationModel.Resources.Core;
                context = new rcns.ResourceContext();
                context.languages = new Array(synthesizer.voice.language);
                resourceMap = rcns.ResourceManager.current.mainResourceMap.getSubtree('LocalizationTTSResources');

                textToSynthesize.innerText = resourceMap.getValue('SynthesizeTextBoundariesDefaultText', context).valueAsString;

                listbox_GetVoices();
                audio_SetUp();

                causeAudioTrackCrash();
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
    var marksMetadataTrackForAudio;

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
            // Clean old cues 
            while (marksMetadataTrackForAudio.cues.length > 0) {
                var cue = marksMetadataTrackForAudio.cues[0];
                marksMetadataTrackForAudio.removeCue(cue);
            }
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
            // Clean old cues 
            while (marksMetadataTrackForAudio.cues.length > 0) {
                var cue = marksMetadataTrackForAudio.cues[0];
                marksMetadataTrackForAudio.removeCue(cue);
            }
            return;
        }

        // Changes the button label. You could also just disable the button if you don't want any user control.
        voicesSelect.disabled = true;
        btnSpeak.innerText = "Stop";
        statusBox.style.backgroundColor = "green";

        if (checkBoxWordBoundaries.checked == true) {
            // Enable cues track generation for word boundaries 
            synthesizer.options.includeWordBoundaryMetadata = true
        }
        else {
            //make sure it is false if unchecked
            synthesizer.options.includeWordBoundaryMetadata = false;
        }

        if (checkBoxSentenceBoundaries.checked == true) {
            // Enable cues track generation for word boundaries 
            synthesizer.options.includeSentenceBoundaryMetadata = true
        }
        else {
            //make sure it is false if unchecked
            synthesizer.options.includeSentenceBoundaryMetadata = false;
        }

        // Creates a stream from the text. This will be played using an audio element.
        synthesizer.synthesizeTextToStreamAsync(textToSynthesize.value).done(
            function (markersStream) {
                // Set the source and start playing the synthesized audio stream.
                var blob = MSApp.createBlobFromRandomAccessStream(markersStream.ContentType, markersStream);
                audio.src = URL.createObjectURL(blob, { oneTimeOnly: true });
                markersStream.seek(0);
                // There is only may be more than one track. We need to set Audio Metadata from all of them
                // Word boundaries will be one track and Sentence boundaries will be another if both are active
                if (markersStream.timedMetadataTracks.length !== 0) {
                        setAudioMetadaTrack(markersStream.timedMetadataTracks);
                }
                audio.play();
            },
            function (error) {
                errorMessage(error.message);
            });
    }

    function causeAudioTrackCrash() {

        var metadataTrackForAudio = audio.addTextTrack("metadata", "SpeechBoundaries");
        metadataTrackForAudio.addEventListener("cuechange", cueChange, false);

        var textCue1 = new TextTrackCue(.101, 250, "first Cue");
        metadataTrackForAudio.addCue(textCue1);
        var textCue2 = new TextTrackCue(.099, 250, "second Cue");
        metadataTrackForAudio.addCue(textCue2);
    }

    // 
    // Event handler for cues being reached by the audio stream being played out. 
    // 

    function cueChange() {
        var cues = marksMetadataTrackForAudio.activeCues;

        if (cues.length > 0) {
            for (var index = 0; index < cues.length; index++) {
                var speechCue = JSON.parse(cues[index].text);

                statusMessage.innerText = speechCue.text;
                // the plus 1 is because it is a 0 based array
                highlightTextOnScreen(speechCue.startPositionInInput, speechCue.endPositionInInput + 1);
                fillTextBoxes(speechCue);

                //depending on the speed of your cues, a wait may be helpfult to make sure it displays.
                wait(200);
            }
        }
    } 

    function sleep(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }

    async function wait(ms) {
        await sleep(ms);
    }

    //populate the correct text boxes with the last boundary hit.
    function fillTextBoxes(speechCue) {
        if (speechCue.cueType == "SpeechWord") {
            lastWordBoundaryText.innerText = speechCue.text;
        }
        if (speechCue.cueType == "SpeechSentence") {
            lastSentenceBoundaryText.innerText = speechCue.text;
        }
    }

    //select the text between the positions to highlight them on the screen
    function highlightTextOnScreen(startPosition, endPosition) {
        textToSynthesize.setSelectionRange(startPosition, endPosition);
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

    // 
    // This function sets the cues from the media track on the speech synthesizer stream to a  
    // HTML5 audio track with media cues converted in json format.  
    function setAudioMetadaTrack(speechMediaTracks) {

        var index = 0;
        var jindex = 0;
        //currently the addTextCue must be called in order so we have to sort the sentence and word boundaries
        //there is a bug open to fix the addTextCue
        if (speechMediaTracks.length == 1) { //we just have sentence OR word boundaries
            for (index = 0; index < speechMediaTracks[0].cues.length; index++) {
                var speechCue = speechMediaTracks[0].cues[index];
                addTextCue(speechCue, speechMediaTracks[0].id);
            }
        }
        else { // we have sentence AND word boundaries
            var speechCue0;
            var speechCue1;
            while (index < speechMediaTracks[0].cues.length && jindex < speechMediaTracks[1].cues.length) {
                if (index < speechMediaTracks[0].cues.length) {
                    speechCue0 = speechMediaTracks[0].cues[index];
                }
                else {
                    speechCue0.startTime = Number.MAX_SAFE_INTEGER;
                }
                if (jindex < speechMediaTracks[1].cues.length) {
                    speechCue1 = speechMediaTracks[1].cues[jindex];
                }
                else {
                    speechCue1.startTime = Number.MAX_SAFE_INTEGER;
                }
                if (speechCue1.startTime < speechCue0.startTime) {//speechCue1 comes first
                    addTextCue(speechCue1, speechMediaTracks[1].id);
                    jindex++;
                }
                else { //speechCue0 comes first
                    addTextCue(speechCue0, speechMediaTracks[0].id);
                    index++;
                }
            }
            while (index < speechMediaTracks[0].cues.length) {
                speechCue0 = speechMediaTracks[0].cues[index];
                addTextCue(speechCue0, speechMediaTracks[0].id);
                index++;
            }
            while (jindex < speechMediaTracks[1].cues.length) {
                speechCue1 = speechMediaTracks[1].cues[jindex];
                addTextCue(speechCue1, speechMediaTracks[1].id);
                jindex++;
            }
        }
    }

    function addTextCue(speechCue, type) {
        //we need to properly clone the data properties and put them in the TextTrackCue to make them available at the event trigger time
        var jsonSpeech = {
            "startPositionInInput": speechCue.startPositionInInput,
            "endPositionInInput": speechCue.endPositionInInput,
            "text": speechCue.text,
            "cueType": type,
            "duration": speechCue.duration
        }
        var textCue = new TextTrackCue(speechCue.startTime / 1000.0, (speechCue.startTime + speechCue.duration + 300) / 1000.0, JSON.stringify(jsonSpeech));
        marksMetadataTrackForAudio.addCue(textCue);

        //git test
    }
})();
