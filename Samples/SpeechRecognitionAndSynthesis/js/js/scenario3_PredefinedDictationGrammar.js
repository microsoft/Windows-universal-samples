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
                    initializeRecognizer();

                    btnListen.addEventListener("click", listenFn, false);
                    btnListenUI.addEventListener("click", listenUIFn, false);
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

    function initializeRecognizer() {
        /// <summary>
        /// Initialize speech recognizer and compile constraints.
        /// </summary>
        recognizer = Windows.Media.SpeechRecognition.SpeechRecognizer();

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

    function listenFn() {
        /// <summary>
        /// Uses the recognizer constructed earlier to listen for speech from the user before displaying 
        /// it back on the screen. Uses developer-provided UI for user feedback.
        /// </summary>
        if (recognizer.state != Windows.Media.SpeechRecognition.SpeechRecognizerState.idle ||
            btnListenUI.disabled == true) { // Check if the recognizer is listening or going into a state to listen.

            btnListen.disabled = true;
            btnListen.innerText = "Stopping recognizer...";

            recognizer.stopRecognitionAsync();
            return;
        }

        // Disable the UI while recognition is occurring and provide feedback to the user about current state.
        btnListenUI.disabled = true;
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
                    btnListen.disabled = false;
                    btnListenUI.disabled = false;
                    btnListen.innerText = "\uE1d6 without UI";
                }
            );
        }
        catch (exception) {
            errorMessage(exception.message);
            btnListen.disabled = false;
            btnListenUI.disabled = false;
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
        errorTextArea.innerText = text;
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
