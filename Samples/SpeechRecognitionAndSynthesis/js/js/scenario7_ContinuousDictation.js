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
    var page = WinJS.UI.Pages.define("/html/scenario7_ContinuousDictation.html", {
        ready: function (element, options) {
            AudioCapturePermissions.requestMicrophonePermission().then(function (available) {
                if (available) {
                    var defaultLang = Windows.Media.SpeechRecognition.SpeechRecognizer.systemSpeechLanguage;

                    var rcns = Windows.ApplicationModel.Resources.Core;
                    context = new rcns.ResourceContext();
                    context.languages = new Array(defaultLang.languageTag);
                    resourceMap = rcns.ResourceManager.current.mainResourceMap.getSubtree('LocalizationSpeechResources');

                    initializeRecognizer(defaultLang);
                    initializeLanguageDropdown();
                    dictatedText = "";

                    btnDictate.addEventListener("click", dictateFn, false); //speak button hit
                    btnClearText.addEventListener("click", clearTextFn, false); //speak button hit
                    languageSelect.addEventListener("change", setLanguageFunction, false);
                }
                else {
                    btnDictate.disabled = true;
                    errorTextArea.innerText = "Microphone Unavailable, check microphone privacy settings.";
                }
            },
            function (error) {
                var messageDialog = new Windows.UI.Popups.MessageDialog(error.message, "Exception");
                messageDialog.showAsync();
            });
        },

        unload: function (element, options) {
            if (recognizer != null) {
                recognizer.removeEventListener('statechanged', onSpeechRecognizerStateChanged, false);
                recognizer.removeEventListener('hypothesisgenerated', onSpeechRecognizerHypothesisGenerated, false);
                recognizer.continuousRecognitionSession.removeEventListener('resultgenerated', onSpeechRecognizerResultGenerated, false);
                recognizer.continuousRecognitionSession.removeEventListener('completed', onSpeechRecognizerSessionCompleted, false);

                recognizer.close();
            }
        }
    });

    var recognizer;
    var dictatedText;

    // localization resources
    var context;
    var resourceMap;

    function initializeRecognizer(language) {
        /// <summary>
        /// Initialize speech recognizer and compile constraints.
        /// </summary>
        if (typeof recognizer !== 'undefined') {
            recognizer = null;
        }
        recognizer = Windows.Media.SpeechRecognition.SpeechRecognizer(language);

        // Provide feedback to the user about the state of the recognizer.
        recognizer.addEventListener('statechanged', onSpeechRecognizerStateChanged, false);
        // Provide feedback to the user about what the recognizer is currently hearing.
        recognizer.addEventListener('hypothesisgenerated', onSpeechRecognizerHypothesisGenerated, false);
        // Handle continuous recognition events. Completed fires when various error states occur or the session otherwise ends.
        // ResultGenerated fires when recognized phrases are spoken or the garbage rule is hit.
        recognizer.continuousRecognitionSession.addEventListener('resultgenerated', onSpeechRecognizerResultGenerated, false);
        recognizer.continuousRecognitionSession.addEventListener('completed', onSpeechRecognizerSessionCompleted, false);

        // Compile the dictation topic constraint, which optimizes for dictated speech.
        var dictationConstraint = new Windows.Media.SpeechRecognition.SpeechRecognitionTopicConstraint(Windows.Media.SpeechRecognition.SpeechRecognitionScenario.dictation, "dictation");
        recognizer.constraints.append(dictationConstraint);

        recognizer.compileConstraintsAsync().done(
            function (result) {
                // Check to make sure that the constraints were in a proper format and the recognizer was able to compile them.
                if (result.status != Windows.Media.SpeechRecognition.SpeechRecognitionResultStatus.success) {
                    btnDictate.disabled = true;
                    btnClearText.disabled = true;
                    // Let the user know that the grammar didn't compile properly.
                    speechRecognizerUnsuccessful(result.status);
                }
                else {
                    btnDictate.disabled = false;
                    btnClearText.disabled = false;
                }
            }
        );
    }

    function initializeLanguageDropdown() {
        /// <summary>
        /// Checks the set of supported languages installed in the system for speech, and adds them
        /// to a dropdown of languages that can be selected from.
        /// </summary>
        var supportedLanguages = Windows.Media.SpeechRecognition.SpeechRecognizer.supportedTopicLanguages;
        for (var langIndex = 0; langIndex < supportedLanguages.size; langIndex++) {
            var lang = supportedLanguages[langIndex];
            var option = document.createElement("option");
            option.text = lang.displayName;
            option.tag = lang;
            languageSelect.add(option, null);
            if (lang.languageTag == recognizer.currentLanguage.languageTag) {
                languageSelect.selectedIndex = langIndex;
            }
        }
    }

    function setLanguageFunction() {
        /// <summary>
        /// When a language is chosen from the dropdown, triggers reinitialization of 
        /// the speech engine, and re-sets the resource map context.
        /// </summary>
        if (languageSelect.selectedIndex !== -1) {
            var option = languageSelect.options[languageSelect.selectedIndex];
            var language = option.tag;

            context.languages = new Array(language.languageTag);
            initializeRecognizer(language);

        }
    }

    function dictateFn() {
        /// <summary>
        /// Begin recognition or finish the recognition session.
        /// </summary>
        btnDictate.disabled = true;

        if (recognizer.state != Windows.Media.SpeechRecognition.SpeechRecognizerState.idle) { // Check if the recognizer is listening or going into a state to listen.
            languageSelect.disabled = false;
            btnDictate.innerText = "Stopping dictation...";

            recognizer.continuousRecognitionSession.stopAsync();
            return;
        }

        btnDictate.innerText = "Stop Dictation";
        languageSelect.disabled = true;
        errorMessage("");

        // Start the continuous recognition session. Results are handled in the event handlers below.
        try {
            recognizer.continuousRecognitionSession.startAsync().then(function completed(){
                btnDictate.disabled = false;
            });
        }
        catch (e) { }
    }

    function clearTextFn() {
        /// <summary>
        /// Clear the dictation textbox.
        /// </summary>
        btnClearText.disabled = true;
        dictatedText = "";
        dictationTextArea.innerText = "";
        errorMessage("");
    }

    function onSpeechRecognizerHypothesisGenerated(eventArgs) {
        /// <summary>
        /// While the user is speaking, update the textbox with the partial sentence of what's being said for user feedback.
        /// </summary>
        var hypothesis = eventArgs.hypothesis.text;

        dictationTextArea.innerText = dictatedText + " " + hypothesis + "...";
        btnClearText.disabled = false;
    }

    function onSpeechRecognizerResultGenerated(eventArgs) {
        /// <summary>
        /// Handle events fired when a result is generated. Check for high to medium confidence, and then append the
        /// string to the end of the previous results. Replace the content of the textbox with the resulting string to
        /// remove any hypothesis text that may be present.
        /// </summary>
        if (eventArgs.result.confidence == Windows.Media.SpeechRecognition.SpeechRecognitionConfidence.high ||
            eventArgs.result.confidence == Windows.Media.SpeechRecognition.SpeechRecognitionConfidence.medium) {

            dictatedText = dictatedText + eventArgs.result.text + " ";
            btnClearText.disabled = false;
        }
        else {
            if (eventArgs.result.text.length != 0) {
                errorTextArea.innerText = "Discarded (low/rejected confidence): " + eventArgs.result.text;
            }
        }
        dictationTextArea.innerText = dictatedText;
    }

    function onSpeechRecognizerSessionCompleted(eventArgs) {
        /// <summary>
        /// Handle events fired when error conditions occur, such as the microphone becoming unavailable, or if
        /// some transient issues occur. This also fires when the session completes normally.
        /// </summary>
        if (eventArgs.status != Windows.Media.SpeechRecognition.SpeechRecognitionResultStatus.success) {
            speechRecognizerUnsuccessful(eventArgs.status);
            dictationTextArea.innerText = dictatedText;
        }
        btnDictate.innerText = "\uE1d6 Dictate";
        btnDictate.disabled = false;
        languageSelect.disabled = false;
    }

    function displayMessage(text) {
        /// <summary>
        /// Sets the status area with the message details and color.
        /// </summary>
        statusMessage.innerText = text;
        statusBox.style.backgroundColor = "green";
    }

    function errorMessage(text) {
        /// <summary>
        /// Sets the specified text area with the error message details.
        /// </summary>
        if (typeof errorTextArea !== "undefined") {
            errorTextArea.innerText = text;
        }
    }

    function onSpeechRecognizerStateChanged(eventArgs) {
        /// <summary>
        /// Looks up the state text and displays the message to the user.
        /// </summary>
        switch (eventArgs.state) {
            case Windows.Media.SpeechRecognition.SpeechRecognizerState.idle: {
                displayMessage("Speech recognizer state: idle");
                break;
            }
            case Windows.Media.SpeechRecognition.SpeechRecognizerState.capturing: {
                displayMessage("Speech recognizer state: capturing");
                break;
            }
            case Windows.Media.SpeechRecognition.SpeechRecognizerState.processing: {
                displayMessage("Speech recognizer state: processing");
                break;
            }
            case Windows.Media.SpeechRecognition.SpeechRecognizerState.soundStarted: {
                displayMessage("Speech recognizer state: soundStarted");
                break;
            }
            case Windows.Media.SpeechRecognition.SpeechRecognizerState.soundEnded: {
                displayMessage("Speech recognizer state: soundEnded");
                break;
            }
            case Windows.Media.SpeechRecognition.SpeechRecognizerState.speechDetected: {
                displayMessage("Speech recognizer state: speechDetected");
                break;
            }
            case Windows.Media.SpeechRecognition.SpeechRecognizerState.paused: {
                displayMessage("Speech recognizer state: paused");
                break;
            }
            default: {
                break;
            }
        }
    }

    function speechRecognizerUnsuccessful(resultStatus) {
        /// <summary>
        /// Looks up the error text and displays the message to the user.
        /// </summary>
        switch (resultStatus) {
            case Windows.Media.SpeechRecognition.SpeechRecognitionResultStatus.audioQualityFailure: {
                errorMessage("Speech recognition error: audioQualityFailure");
                break;
            }
            case Windows.Media.SpeechRecognition.SpeechRecognitionResultStatus.grammarCompilationFailure: {
                errorMessage("Speech recognition error: grammarCompilationFailure");
                break;
            }
            case Windows.Media.SpeechRecognition.SpeechRecognitionResultStatus.grammarLanguageMismatch: {
                errorMessage("Speech recognition error: grammarLanguageMismatch");
                break;
            }
            case Windows.Media.SpeechRecognition.SpeechRecognitionResultStatus.microphoneUnavailable: {
                errorMessage("Speech recognition error: microphoneUnavailable");
                break;
            }
            case Windows.Media.SpeechRecognition.SpeechRecognitionResultStatus.networkFailure: {
                errorMessage("Speech recognition error: networkFailure");
                break;
            }
            case Windows.Media.SpeechRecognition.SpeechRecognitionResultStatus.pauseLimitExceeded: {
                errorMessage("Speech recognition error: pauseLimitExceeded");
                break;
            }
            case Windows.Media.SpeechRecognition.SpeechRecognitionResultStatus.timeoutExceeded: {
                errorMessage("Speech recognition error: timeoutExceeded");
                break;
            }
            case Windows.Media.SpeechRecognition.SpeechRecognitionResultStatus.topicLanguageNotSupported: {
                errorMessage("Speech recognition error: topicLanguageNotSupported");
                break;
            }
            case Windows.Media.SpeechRecognition.SpeechRecognitionResultStatus.unknown: {
                errorMessage("Speech recognition error: unknown");
                break;
            }
            case Windows.Media.SpeechRecognition.SpeechRecognitionResultStatus.userCanceled: {
                errorMessage("Recognition canceled by the user.");
                break;
            }
            default: {
                break;
            }
        }
    }
})();
