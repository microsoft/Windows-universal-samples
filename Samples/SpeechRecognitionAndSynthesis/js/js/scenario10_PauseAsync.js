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
    var page = WinJS.UI.Pages.define("/html/scenario10_PauseAsync.html", {
        ready: function (element, options) {
            AudioCapturePermissions.requestMicrophonePermission().then(function (available) {
                if (available) {
                    initializeRecognizer();

                    btnRecognize.addEventListener("click", continuousRecoFn, false); //speak button click
                    btnEmailGrammar.addEventListener("click", emailGrammarFn, false); //email grammar button click
                    btnPhoneGrammar.addEventListener("click", phoneGrammarFn, false); //phone grammar button click
                }
                else {
                    btnRecognize.disabled = true;
                    errorMessage("Microphone Unavailable, check microphone privacy settings.");
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

    // These speech recognition constraints will be added and removed from the recognizer.
    var emailConstraint;
    var phoneConstraint;

    function initializeRecognizer() {
        /// <summary>
        /// Initialize speech recognizer and compile constraints.
        /// </summary>
        /// <remarks>
        /// For more information about grammars for Windows apps and how to
        /// define and use SRGS-compliant grammars in your app, see
        /// https://msdn.microsoft.com/en-us/library/windows/apps/xaml/dn596121.aspx
        /// </remarks>
        recognizer = Windows.Media.SpeechRecognition.SpeechRecognizer();

        // Provide feedback to the user about the state of the recognizer.
        recognizer.addEventListener('statechanged', onSpeechRecognizerStateChanged, false);
        // Handle continuous recognition events. Completed fires when various error states occur or the session otherwise ends.
        // ResultGenerated fires when recognized phrases are spoken or the garbage rule is hit.
        recognizer.continuousRecognitionSession.addEventListener('resultgenerated', onSpeechRecognizerResultGenerated, false);
        recognizer.continuousRecognitionSession.addEventListener('completed', onSpeechRecognizerSessionCompleted, false);

        // It's not valid to pause a list grammar recognizer and recompile the constraints without at least one
        // constraint in place, so create a permanent constraint.
        var goHomeConstraint = new Windows.Media.SpeechRecognition.SpeechRecognitionListConstraint(["Go home"], "goHome");

        emailConstraint = new Windows.Media.SpeechRecognition.SpeechRecognitionListConstraint(["Send email"], "email");
        phoneConstraint = new Windows.Media.SpeechRecognition.SpeechRecognitionListConstraint(["Call phone"], "phone");

        recognizer.constraints.append(goHomeConstraint);
        recognizer.constraints.append(emailConstraint);

        recognizer.compileConstraintsAsync().done(
            function (result) {
                // Check to make sure that the constraints were in a proper format and the recognizer was able to compile them.
                if (result.status != Windows.Media.SpeechRecognition.SpeechRecognitionResultStatus.success) {
                    btnRecognize.disabled = true;
                    // Let the user know that the grammar didn't compile properly.
                    speechRecognizerUnsuccessful(result.status);
                }
                else {
                    btnRecognize.disabled = false;
                }
            }
        );
    }

    function continuousRecoFn() {
        /// <summary>
        /// Begin recognition or finish the recognition session.
        /// </summary>

        // Check if the recognizer is listening or going into a state to listen.
        if (recognizer.state != Windows.Media.SpeechRecognition.SpeechRecognizerState.idle) {
            disableUI(true);
            btnRecognize.innerText = "Stopping recognition...";

            recognizer.continuousRecognitionSession.cancelAsync();
        }
        else {
            btnRecognize.innerText = "\uE1d6 Stop recognition";
            errorMessage("");

            // Start the continuous recognition session. Results are handled in the event handlers below.
            try {
                recognizer.continuousRecognitionSession.startAsync().then(function completed() {
                    disableUI(false);
                });
            }
            catch (e) {
                disableUI(true);

            }
        }
    }

    function emailGrammarFn() {
        /// <summary>
        /// Adds or removes the 'email' constraint from the recognizer.
        /// </summary>

        // Update UI, disabling buttons so the user can't interrupt.
        disableUI(true);
        var newButtonText = "";
        var start = performance.now();

        try {
            recognizer.continuousRecognitionSession.pauseAsync().then(
                function (result) {
                    // Update the grammar appropriately.
                    if (btnEmailGrammar.innerText == "Remove 'email' grammar") {
                        btnEmailGrammar.innerText = "Updating...";

                        var index = recognizer.constraints.indexOf(emailConstraint);
                        if (index.index != -1) {
                            recognizer.constraints.removeAt(index.index);
                        }

                        newButtonText = "Add 'email' grammar";
                        emailInfoTextBlock.style.visibility = "hidden";
                    }
                    else {
                        btnEmailGrammar.innerText = "Updating...";
                        recognizer.constraints.append(emailConstraint);
                        newButtonText = "Remove 'email' grammar";
                        emailInfoTextBlock.style.visibility = "visible";
                    }

                    return recognizer.compileConstraintsAsync();
                }
            ).then(
                function (result) {
                    // Check to make sure that the constraints were in a proper format and the recognizer was able to compile them.
                    if (result.status != Windows.Media.SpeechRecognition.SpeechRecognitionResultStatus.success) {
                        disableUI(true);

                        // Let the user know that the grammar didn't compile properly.
                        speechRecognizerUnsuccessful(result.status);

                        recognizer.continuousRecognitionSession.cancelAsync();
                    }
                    else {
                        recognizer.continuousRecognitionSession.resume();

                        var end = performance.now();
                        var elapsed = end - start;
                        msTextBlock.innerText = "Grammar update took " + elapsed + " ms";

                        // Restore the original UI state.
                        disableUI(false);
                        btnEmailGrammar.innerText = newButtonText;
                    }
                }
            );
        }
        catch (e) {
            var messageDialog = new Windows.UI.Popups.MessageDialog(e.message, "Exception");
            messageDialog.showAsync();
        }
    }

    function phoneGrammarFn() {
        /// <summary>
        /// Adds or removes the 'phone' constraint from the recognizer.
        /// </summary>

        // Update UI, disabling buttons so the user can't interrupt.
        disableUI(true);
        var newButtonText = "";
        var start = performance.now();

        try {
            recognizer.continuousRecognitionSession.pauseAsync().then(
                function (result) {
                    // Update the grammar appropriately.
                    if (btnPhoneGrammar.innerText == "Remove 'phone' grammar") {
                        btnPhoneGrammar.innerText = "Updating...";

                        var index = recognizer.constraints.indexOf(phoneConstraint);
                        if (index.index != -1) {
                            recognizer.constraints.removeAt(index.index);
                        }

                        newButtonText = "Add 'phone' grammar";
                        phoneInfoTextBlock.style.visibility = "hidden";
                    }
                    else {
                        btnPhoneGrammar.innerText = "Updating...";
                        recognizer.constraints.append(phoneConstraint);
                        newButtonText = "Remove 'phone' grammar";
                        phoneInfoTextBlock.style.visibility = "visible";
                    }

                    return recognizer.compileConstraintsAsync();
                }
            ).then(
                function (result) {
                    // Check to make sure that the constraints were in a proper format and the recognizer was able to compile them.
                    if (result.status != Windows.Media.SpeechRecognition.SpeechRecognitionResultStatus.success) {
                        disableUI(true);

                        // Let the user know that the grammar didn't compile properly.
                        speechRecognizerUnsuccessful(result.status);

                        recognizer.continuousRecognitionSession.cancelAsync();
                    }
                    else {
                        recognizer.continuousRecognitionSession.resume();

                        var end = performance.now();
                        var elapsed = end - start;
                        msTextBlock.innerText = "Grammar update took " + elapsed + " ms";

                        // Restore the original UI state.
                        disableUI(false);
                        btnPhoneGrammar.innerText = newButtonText;
                    }
                }
            );
        }
        catch (e) {
            var messageDialog = new Windows.UI.Popups.MessageDialog(e.message, "Exception");
            messageDialog.showAsync();
        }
    }

    function onSpeechRecognizerResultGenerated(eventArgs) {
        /// <summary>
        /// Handle events fired when a result is generated. This may include a garbage rule that fires when general room noise
        /// or side-talk is captured (this will have a confidence of rejected typically, but may occasionally match a rule with
        /// low confidence).
        /// </summary>
        /// <param type="SpeechContinuousRecognitionResultGeneratedEventArgs" name="eventArgs">
        /// Details about the recognized speech.
        /// </param>

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

            resultTextBlock.innerText = "Heard: " + eventArgs.result.text + ", (Tag: '" + tag + "', Confidence: " + convertConfidenceToString(eventArgs.result.confidence) + ")";
        }
        else {
            resultTextBlock.innerText = "Sorry, I didn't catch that. (Heard: " + eventArgs.result.text + ", Tag: '" + tag + "', Confidence: " + convertConfidenceToString(eventArgs.result.confidence) + ")";
        }
    }

    function onSpeechRecognizerSessionCompleted(eventArgs) {
        /// <summary>
        /// Handle events fired when error conditions occur, such as the microphone becoming unavailable, or if
        /// some transient issues occur. This also fires when the session completes normally.
        /// </summary>
        /// <param type="SpeechContinuousRecognitionCompletedEventArgs" name="eventArgs">
        /// The state of the recognizer.
        /// </param>
        if (eventArgs.status != Windows.Media.SpeechRecognition.SpeechRecognitionResultStatus.success) {
            speechRecognizerUnsuccessful(eventArgs.status);
        }
        btnRecognize.innerText = "\uE1d6 Continuous Recognition";
        btnRecognize.disabled = false;
    }

    function displayMessage(text) {
        /// <summary>
        /// Sets the status area with the message details and color.
        /// </summary>
        /// <param type="string" name="text">
        /// The text to be displayed in the status box.
        /// </param>
        statusMessage.innerText = text;
        statusBox.style.backgroundColor = "green";
    }

    function errorMessage(text) {
        /// <summary>
        /// Sets the specified text area with the error message details.
        /// </summary>
        /// <param type="string" name="text">
        /// The error text to be displayed to the user.
        /// </param>
        if (typeof errorTextArea !== "undefined") {
            errorTextArea.innerText = text;
        }
    }

    function disableUI(newState) {
        /// <summary>
        /// Enables/disables the UI based on the passed in boolean.
        /// </summary>
        /// <param type="bool" name="newState">
        /// The new disabled state for the UI.
        /// </param>
        btnRecognize.disabled = newState;
        btnEmailGrammar.disabled = newState;
        btnPhoneGrammar.disabled = newState;
    }

    function onSpeechRecognizerStateChanged(eventArgs) {
        /// <summary>
        /// Looks up the state text and displays the message to the user.
        /// </summary>
        /// <param type="SpeechRecognizerStateChangedEventArgs" name="eventArgs">
        /// The state of the recognizer.
        /// </param>
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
        /// <param type="SpeechRecognitionResultStatus" name="resultStatus">
        /// The status of the speech recognition result.
        /// </param>
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
        /// </summary>
        /// <param name="confidence">
        /// The numeric confidence returned by SpeechRecognitionResult.Confidence.
        /// </param>
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
