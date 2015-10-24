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
    var page = WinJS.UI.Pages.define("/html/scenario3_PredefinedDictationGrammar.html", {
        ready: function (element, options) {
            AudioCapturePermissions.requestMicrophonePermission().then(function (available) {
                if (available) {
                    var defaultLang = Windows.Media.SpeechRecognition.SpeechRecognizer.systemSpeechLanguage;
                    initializeRecognizer(defaultLang);
                    initializeLanguageDropdown();

                    btnListen.addEventListener("click", listenFn, false);
                    btnListenUI.addEventListener("click", listenUIFn, false);
                    languageSelect.addEventListener("change", setLanguageFunction, false);

                    var rcns = Windows.ApplicationModel.Resources.Core;
                    context = new rcns.ResourceContext();
                    context.languages = new Array(defaultLang.languageTag);
                    resourceMap = rcns.ResourceManager.current.mainResourceMap.getSubtree('LocalizationSpeechResources');
                }
                else {
                    btnListen.disabled = true;
                    btnListenUI.disabled = true;
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

                recognizer.close();
            }
        }
    });

    var recognizer;

    // localization resources
    var context;
    var resourceMap;

    function initializeRecognizer(language) {
        /// <summary>
        /// Initialize speech recognizer and compile constraints.
        /// </summary>
        if (typeof recognizer !== 'undefined')
        {
            recognizer = null;
        }

        recognizer = Windows.Media.SpeechRecognition.SpeechRecognizer(language);

        // Provide feedback to the user about the state of the recognizer.
        recognizer.addEventListener('statechanged', onSpeechRecognizerStateChanged, false);
        

        // Compile the dictation topic constraint, which optimizes for dictated speech.
        var dictationConstraint = new Windows.Media.SpeechRecognition.SpeechRecognitionTopicConstraint(Windows.Media.SpeechRecognition.SpeechRecognitionScenario.dictation, "dictation");
        recognizer.constraints.append(dictationConstraint);

        recognizer.compileConstraintsAsync().done(
            function (result) {
                // Check to make sure that the constraints were in a proper format and the recognizer was able to compile them.
                if (result.status != Windows.Media.SpeechRecognition.SpeechRecognitionResultStatus.success) {
                    btnListen.disabled = true;
                    btnListenUI.disabled = true;
                    // Let the user know that the grammar didn't compile properly.
                    speechRecognizerUnsuccessful(result.status);
                }
                else {
                    btnListen.disabled = false;
                    btnListenUI.disabled = false;
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
        for (var langIndex = 0; langIndex < supportedLanguages.size; langIndex++)
        {
            var lang = supportedLanguages[langIndex];
            var option = document.createElement("option");
            option.text = lang.displayName;
            option.tag = lang;
            languageSelect.add(option, null);
            if(lang.languageTag == recognizer.currentLanguage.languageTag)
            {
                languageSelect.selectedIndex = langIndex;
            }
        }
    }

    function setLanguageFunction() {
        /// <summary>
        /// When a language is chosen from the dropdown, triggers reinitialization of 
        /// the speech engine, and re-sets the resource map context.
        /// </summary>
        if(languageSelect.selectedIndex !== -1)
        {
            var option = languageSelect.options[languageSelect.selectedIndex];
            var language = option.tag;
            context.languages = new Array(language.languageTag);

            initializeRecognizer(language);
        }
    }

    function listenFn() {
        /// <summary>
        /// Uses the recognizer constructed earlier to listen for speech from the user before displaying 
        /// it back on the screen. Uses developer-provided UI for user feedback.
        /// </summary>
        if (recognizer.state != Windows.Media.SpeechRecognition.SpeechRecognizerState.idle ||
            btnListenUI.disabled == true) { // Check if the recognizer is listening or going into a state to listen.

            btnListen.disabled = true;
            languageSelect.disabled = false;
            btnListen.innerText = "Stopping recognizer...";

            recognizer.stopRecognitionAsync();
            return;
        }

        // Disable the UI while recognition is occurring and provide feedback to the user about current state.
        btnListenUI.disabled = true;
        languageSelect.disabled = true;
        btnListen.innerText = "Listening for speech. Click to stop.";
        errorMessage("");

        // Start recognition.
        try {
            recognizer.recognizeAsync().then(
                function (result) {
                    // If successful, display the recognition result.
                    if (result.status == Windows.Media.SpeechRecognition.SpeechRecognitionResultStatus.success) {
                        resultTextArea.innerText = result.text;
                    }
                    else {
                        speechRecognizerUnsuccessful(result.status);
                    }
                },
                function (error) {
                    errorMessage(error.message);
                }
            ).done(
                function (result) {
                    // Reset UI state.
                    if (typeof btnListen !== "undefined") {
                        btnListen.disabled = false;
                        btnListenUI.disabled = false;
                        languageSelect.disabled = false;
                        btnListen.innerText = "\uE1d6 without UI";
                    }
                }
            );
        }
        catch (exception) {
            errorMessage(exception.message);
            btnListen.disabled = false;
            btnListenUI.disabled = false;
            languageSelect.disabled = false;
            btnListen.innerText = "\uE1d6 without UI";
        }
    }

    function listenUIFn() {
        /// <summary>
        /// Uses the recognizer constructed earlier to listen for speech from the user before displaying 
        /// it back on the screen. Uses the built-in speech recognition UI.
        /// </summary>
        btnListen.disabled = true;
        btnListenUI.disabled = true;
        errorMessage("");

        // Start recognition.
        try {
            recognizer.recognizeWithUIAsync().then(
                function (result) {
                    // If successful, display the recognition result.
                    if (result.status == Windows.Media.SpeechRecognition.SpeechRecognitionResultStatus.success) {
                        resultTextArea.innerText = result.text;
                    }
                    else {
                        speechRecognizerUnsuccessful(result.status);
                    }
                },
                function (error) {
                    errorMessage(error.message);
                }
            ).done(
                function (result) {
                    btnListen.disabled = false;
                    btnListenUI.disabled = false;
                }
            );
        }
        catch (exception) {
            errorMessage(exception.message);

            btnListen.disabled = false;
            btnListenUI.disabled = false;
        }
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
