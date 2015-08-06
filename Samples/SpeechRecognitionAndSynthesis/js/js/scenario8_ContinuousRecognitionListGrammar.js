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
    var page = WinJS.UI.Pages.define("/html/scenario8_ContinuousRecognitionListGrammar.html", {
        ready: function (element, options) {
            AudioCapturePermissions.requestMicrophonePermission().then(function (available) {
                if (available) {
                    initializeRecognizer();
                    btnContinuousReco.addEventListener("click", continuousRecoFn, false);
                }
                else {
                    btnContinuousReco.disabled = true;
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
                recognizer.continuousRecognitionSession.removeEventListener('resultgenerated', onSpeechRecognizerResultGenerated, false);
                recognizer.continuousRecognitionSession.removeEventListener('completed', onSpeechRecognizerSessionCompleted, false);

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
        // Handle continuous recognition events. Completed fires when various error states occur or the session otherwise ends.
        // ResultGenerated fires when recognized phrases are spoken or the garbage rule is hit.
        recognizer.continuousRecognitionSession.addEventListener('resultgenerated', onSpeechRecognizerResultGenerated, false);
        recognizer.continuousRecognitionSession.addEventListener('completed', onSpeechRecognizerSessionCompleted, false);

        // Build a command-list grammar. Multiple commands can be given the same tag, allowing for variations on the same command to be handled easily.
        recognizer.constraints.append(Windows.Media.SpeechRecognition.SpeechRecognitionListConstraint(["Go Home"], "Home"));
        recognizer.constraints.append(Windows.Media.SpeechRecognition.SpeechRecognitionListConstraint(["Go to Contoso Studio"], "GoToContosoStudio"));
        recognizer.constraints.append(Windows.Media.SpeechRecognition.SpeechRecognitionListConstraint(["Show Message", "Open Message"], "Message"));
        recognizer.constraints.append(Windows.Media.SpeechRecognition.SpeechRecognitionListConstraint(["Send Email", "Create Email"], "Email"));
        recognizer.constraints.append(Windows.Media.SpeechRecognition.SpeechRecognitionListConstraint(["Call Nita Farley", "Call Nita"], "CallNita"));
        recognizer.constraints.append(Windows.Media.SpeechRecognition.SpeechRecognitionListConstraint(["Call Wayne Sigmon ", "Call Wayne"], "CallWayne"));

        recognizer.compileConstraintsAsync().done(
            function (result) {
                // Check to make sure that the constraints were in a proper format and the recognizer was able to compile them.
                if (result.status != Windows.Media.SpeechRecognition.SpeechRecognitionResultStatus.success) {
                    btnContinuousReco.disabled = true;
                    // Let the user know that the grammar didn't compile properly.
                    speechRecognizerUnsuccessful(result.status);
                }
                else {
                    btnContinuousReco.disabled = false;
                }
            }
        );
    }

    function continuousRecoFn() {
        /// <summary>
        /// Begin recognition or finish the recognition session.
        /// </summary>
        if (recognizer.state != Windows.Media.SpeechRecognition.SpeechRecognizerState.idle) { // Check if the recognizer is listening or going into a state to listen.
            btnContinuousReco.disabled = true;
            btnContinuousReco.innerText = "Stopping recognition...";

            recognizer.continuousRecognitionSession.stopAsync();
            return;
        }

        btnContinuousReco.innerText = "Stop recognition";
        errorMessage("");

        // Start the continuous recognition session. Results are handled in the event handlers below.
        try {
            recognizer.continuousRecognitionSession.startAsync();
        }
        catch (e) { }
    }

    function onSpeechRecognizerResultGenerated(eventArgs) {
        /// <summary>
        /// Handle events fired when a result is generated. This may include a garbage rule that fires when general room noise
        /// or side-talk is captured (this will have a confidence of rejected typically, but may occasionally match a rule with
        /// low confidence).
        /// </summary>

        // The garbage rule will not have a tag associated with it, the other rules will return a string matching the tag provided
        // when generating the grammar.
        var tag = "unknown";
        if (eventArgs.result.constraint != null) {
            tag = eventArgs.result.constraint.tag;
        }

        // Developers may decide to use per-phrase confidence levels in order to tune the behavior of their 
        // grammar based on testing.
        if (eventArgs.result.confidence == Windows.Media.SpeechRecognition.SpeechRecognitionConfidence.high ||
            eventArgs.result.confidence == Windows.Media.SpeechRecognition.SpeechRecognitionConfidence.medium) {

            resultTextArea.innerText = "Heard: " + eventArgs.result.text + ", (Tag: '" + tag + "', Confidence: " + convertConfidenceToString(eventArgs.result.confidence) + ")";
        }
        else {
            resultTextArea.innerText = "Sorry, I didn't catch that. (Heard: " + eventArgs.result.text + ", Tag: '" + tag + "', Confidence: " + convertConfidenceToString(eventArgs.result.confidence) + ")";
        }
    }

    function onSpeechRecognizerSessionCompleted(eventArgs) {
        /// <summary>
        /// Handle events fired when error conditions occur, such as the microphone becoming unavailable, or if
        /// some transient issues occur. This also fires when the session completes normally.
        /// </summary>
        if (eventArgs.status != Windows.Media.SpeechRecognition.SpeechRecognitionResultStatus.success) {
            speechRecognizerUnsuccessful(eventArgs.status);
        }
        btnContinuousReco.innerText = "\uE1d6 Continuous Recognition";
        btnContinuousReco.disabled = false;
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

    function convertConfidenceToString(confidence) {
        /// <summary> Converts numeric confidence value into text representation of
        /// Windows.Media.SpeechRecognition.SpeechRecognitionConfidence for visualization.
        /// <param name="confidence">The numeric confidence returned by SpeechRecognitionResult.Confidence</param>
        /// </summary>
        switch (confidence) {
            case Windows.Media.SpeechRecognition.SpeechRecognitionConfidence.high: {
                return "high";
            }
            case Windows.Media.SpeechRecognition.SpeechRecognitionConfidence.medium: {
                return "medium";
            }
            case Windows.Media.SpeechRecognition.SpeechRecognitionConfidence.low: {
                return "low";
            }
            case Windows.Media.SpeechRecognition.SpeechRecognitionConfidence.rejected: {
                return "rejected";
            }
            default:
                return "unknown";
        }
    }
})();
