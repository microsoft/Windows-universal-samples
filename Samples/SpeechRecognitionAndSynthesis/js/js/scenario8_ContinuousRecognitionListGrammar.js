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
                    var defaultLang = Windows.Media.SpeechRecognition.SpeechRecognizer.systemSpeechLanguage;

                    var rcns = Windows.ApplicationModel.Resources.Core;
                    context = new rcns.ResourceContext();
                    context.languages = new Array(defaultLang.languageTag);
                    resourceMap = rcns.ResourceManager.current.mainResourceMap.getSubtree('LocalizationSpeechResources');

                    initializeRecognizer(defaultLang);
                    initializeLanguageDropdown();

                    btnContinuousReco.addEventListener("click", continuousRecoFn, false);
                    languageSelect.addEventListener("change", setLanguageFunction, false);
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
        // Handle continuous recognition events. Completed fires when various error states occur or the session otherwise ends.
        // ResultGenerated fires when recognized phrases are spoken or the garbage rule is hit.
        recognizer.continuousRecognitionSession.addEventListener('resultgenerated', onSpeechRecognizerResultGenerated, false);
        recognizer.continuousRecognitionSession.addEventListener('completed', onSpeechRecognizerSessionCompleted, false);

        // Build a command-list grammar. Multiple commands can be given the same tag, allowing for variations on the same command to be handled easily.
        recognizer.constraints.append(
            Windows.Media.SpeechRecognition.SpeechRecognitionListConstraint([
                resourceMap.getValue('ListGrammarGoHome', context).valueAsString
            ], "Home"));
        recognizer.constraints.append(
            Windows.Media.SpeechRecognition.SpeechRecognitionListConstraint([
                resourceMap.getValue('ListGrammarGoToContosoStudio', context).valueAsString
            ], "GoToContosoStudio"));
        recognizer.constraints.append(
            Windows.Media.SpeechRecognition.SpeechRecognitionListConstraint([
                resourceMap.getValue('ListGrammarShowMessage', context).valueAsString,
                resourceMap.getValue('ListGrammarOpenMessage', context).valueAsString
            ], "Message"));
        recognizer.constraints.append(
            Windows.Media.SpeechRecognition.SpeechRecognitionListConstraint([
                resourceMap.getValue('ListGrammarSendEmail', context).valueAsString,
                resourceMap.getValue('ListGrammarCreateEmail', context).valueAsString
            ], "Email"));
        recognizer.constraints.append(
            Windows.Media.SpeechRecognition.SpeechRecognitionListConstraint([
                resourceMap.getValue('ListGrammarCallNitaFarley', context).valueAsString,
                resourceMap.getValue('ListGrammarCallNita', context).valueAsString
            ], "CallNita"));
        recognizer.constraints.append(
            Windows.Media.SpeechRecognition.SpeechRecognitionListConstraint([
                resourceMap.getValue('ListGrammarCallWayneSigmon', context).valueAsString,
                resourceMap.getValue('ListGrammarCallWayne', context).valueAsString
            ], "CallWayne"));


        // RecognizeWithUIAsync allows developers to customize the prompts.
        var helpString = "Try saying '" +
            resourceMap.getValue('ListGrammarGoHome', context).valueAsString + "', '" +
            resourceMap.getValue('ListGrammarGoToContosoStudio', context).valueAsString + "' or '" +
            resourceMap.getValue('ListGrammarShowMessage', context).valueAsString + "'";
        
        helpText.innerHTML = resourceMap.getValue('ListGrammarHelpText', context).valueAsString + "<br/>" + helpString;


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

    function continuousRecoFn() {
        /// <summary>
        /// Begin recognition or finish the recognition session.
        /// </summary>
        btnContinuousReco.disabled = true;
        if (recognizer.state != Windows.Media.SpeechRecognition.SpeechRecognizerState.idle) { // Check if the recognizer is listening or going into a state to listen.
            btnContinuousReco.innerText = "Stopping recognition...";
            languageSelect.disabled = false;

            recognizer.continuousRecognitionSession.stopAsync();
            return;
        }

        btnContinuousReco.innerText = "Stop recognition";
        languageSelect.disabled = true;
        errorMessage("");

        // Start the continuous recognition session. Results are handled in the event handlers below.
        try {
            recognizer.continuousRecognitionSession.startAsync().then(function completed() {
                btnContinuousReco.disabled = false;
            });
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
