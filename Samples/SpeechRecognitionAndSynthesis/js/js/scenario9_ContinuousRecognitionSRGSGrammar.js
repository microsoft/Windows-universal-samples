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
    var page = WinJS.UI.Pages.define("/html/scenario9_ContinuousRecognitionSRGSGrammar.html", {
        ready: function (element, options) {
            AudioCapturePermissions.requestMicrophonePermission().then(function (available) {
                if (available) {
                    var c = document.getElementById("shapes");
                    shapeCanvas = c.getContext("2d");

                    var defaultLang = Windows.Media.SpeechRecognition.SpeechRecognizer.systemSpeechLanguage;

                    var rcns = Windows.ApplicationModel.Resources.Core;
                    context = new rcns.ResourceContext();
                    context.languages = new Array(defaultLang.languageTag);
                    resourceMap = rcns.ResourceManager.current.mainResourceMap.getSubtree('LocalizationSpeechResources');

                    initializeRecognizer(defaultLang);
                    initializeLanguageDropdown();
                    drawCanvas();

                    btnContinuousReco.addEventListener("click", continuousRecoFn, false); //speak button hit
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
    var shapeCanvas;
    var borderColor = "#FFFFFF";
    var backgroundColor = "#808080";
    var circleColor = "#000000";
    var colorLookup = {};
    colorLookup["COLOR_RED"] = "#FF0000";
    colorLookup["COLOR_BLUE"] = "#0000FF";
    colorLookup["COLOR_BLACK"] = "#000000";
    colorLookup["COLOR_BROWN"] = "#A52A2A";
    colorLookup["COLOR_PURPLE"] = "#800080";
    colorLookup["COLOR_GREEN"] = "#008000";
    colorLookup["COLOR_YELLOW"] = "#FFFF00";
    colorLookup["COLOR_CYAN"] = "#00FFFF";
    colorLookup["COLOR_MAGENTA"] = "#FF00FF";
    colorLookup["COLOR_ORANGE"] = "#FFA500";
    colorLookup["COLOR_GRAY"] = "#808080";
    colorLookup["COLOR_WHITE"] = "#FFFFFF";

    // localization resources
    var context;
    var resourceMap;

    function initializeRecognizer(language) {
        /// <summary>
        /// Initialize speech recognizer and compile constraints.
        /// </summary>
        /// <remarks>
        /// For more information about grammars for Windows apps and how to
        /// define and use SRGS-compliant grammars in your app, see
        /// https://msdn.microsoft.com/en-us/library/dn596121.aspx
        /// </remarks>
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

        // Update help text.
        resultTextArea.innerText = resourceMap.getValue("SRGSListeningPromptText", context).valueAsString;

        // Specify the location of your grammar file, then retrieve it as a StorageFile.
        var grammarFilePath = "SRGS\\" + language.languageTag + "\\SRGSColors.xml";

        Windows.ApplicationModel.Package.current.installedLocation.getFileAsync(grammarFilePath).done(
            function (result) {
                var fileConstraint = new Windows.Media.SpeechRecognition.SpeechRecognitionGrammarFileConstraint(result, "colors");
                recognizer.constraints.append(fileConstraint);

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
        // Check if the recognizer is listening or going into a state to listen.
        if (recognizer.state != Windows.Media.SpeechRecognition.SpeechRecognizerState.idle) {
            languageSelect.disabled = false;
            btnContinuousReco.innerText = "Stopping recognition...";

            recognizer.continuousRecognitionSession.stopAsync();
            return;
        }

        btnContinuousReco.innerText = "Stop recognition";
        languageSelect.disabled = true;
        errorMessage("");

        // Update help text.
        resultTextArea.innerText = resourceMap.getValue("SRGSListeningPromptText", context).valueAsString;

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
        /// Handle events fired when a result is generated.
        /// </summary>
        handleRecognitionResult(eventArgs.result);
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

    function handleRecognitionResult(result) {
        /// <summary>
        /// Uses the result from the speech recognizer to change the colors of the shapes.
        /// </summary>
        /// <param name="result">The result from the recognition event.</param>

        // Check the confidence level of the recognition result.
        if (result.confidence == Windows.Media.SpeechRecognition.SpeechRecognitionConfidence.high ||
            result.confidence == Windows.Media.SpeechRecognition.SpeechRecognitionConfidence.medium) {

            var semanticsObj = new Object(result.semanticInterpretation);

            // Declare a string that will contain messages when the color rule matches GARBAGE.
            var garbagePrompt = "";

            // BACKGROUND: Check to see if the recognition result contains the semantic key for the background color,
            // and not a match for the GARBAGE rule, and change the color.
            if ("KEY_BACKGROUND" in semanticsObj.properties && semanticsObj.properties["KEY_BACKGROUND"].toString() != "...") {
                var newBackgroundColor = semanticsObj.properties["KEY_BACKGROUND"].toString();
                backgroundColor = getColor(newBackgroundColor);
            }

            // If "background" was matched, but the color rule matched GARBAGE, prompt the user.
            else if ("KEY_BACKGROUND" in semanticsObj.properties && semanticsObj.properties["KEY_BACKGROUND"].toString() == "...") {

                garbagePrompt += "Didn't get the background color \n\nTry saying blue background\n";
                resultTextArea.innerText = garbagePrompt;
            }

            // BORDER: Check to see if the recognition result contains the semantic key for the border color,
            // and not a match for the GARBAGE rule, and change the color.
            if ("KEY_BORDER" in semanticsObj.properties && semanticsObj.properties["KEY_BORDER"].toString() != "...") {
                var newBorderColor = semanticsObj.properties["KEY_BORDER"].toString();
                borderColor = getColor(newBorderColor);
            }

            // If "border" was matched, but the color rule matched GARBAGE, prompt the user.
            else if ("KEY_BORDER" in semanticsObj.properties && semanticsObj.properties["KEY_BORDER"].toString() == "...") {
                garbagePrompt += "Didn't get the border color\n\n Try saying red border\n";
                resultTextArea.innerText = garbagePrompt;
            }

            // CIRCLE: Check to see if the recognition result contains the semantic key for the circle color,
            // and not a match for the GARBAGE rule, and change the color.
            if ("KEY_CIRCLE" in semanticsObj.properties && semanticsObj.properties["KEY_CIRCLE"].toString() != "...") {
                var newCircleColor = semanticsObj.properties["KEY_CIRCLE"].toString();
                circleColor = getColor(newCircleColor);
            }

            // If "circle" was matched, but the color rule matched GARBAGE, prompt the user.
            else if ("KEY_CIRCLE" in semanticsObj.properties && semanticsObj.properties["KEY_CIRCLE"].toString() == "...") {
                garbagePrompt += "Didn't get the circle color\n\n Try saying green circle\n";
                resultTextArea.innerText = garbagePrompt;
            }

            // Initialize a string that will describe the user's color choices.
            var textBoxColors = "You selected (Semantic Interpretation)-> \n";

            // Write the color choices contained in the semantics of the recognition result to the text box.
            for (var key in semanticsObj.properties) {
                if (semanticsObj.properties.hasOwnProperty(key)) {
                    var value = semanticsObj.properties[key];

                    // Check to see if any of the semantic values in recognition result contains a match for the GARBAGE rule.
                    if (value != "...") {

                        // Cycle through the semantic keys and values and write them to the text box.
                        textBoxColors += value;
                        textBoxColors += " ";
                        textBoxColors += (key || "null");
                        textBoxColors += "\n";

                        resultTextArea.innerText = textBoxColors;
                    }
                    // If there was no match to the colors rule or if it matched GARBAGE, prompt the user.
                    else {
                        resultTextArea.innerText = garbagePrompt;
                    }
                }
            }

            drawCanvas();
        }

        // Prompt the user if recognition failed or recognition confidence is low.
        else if (result.confidence == Windows.Media.SpeechRecognition.SpeechRecognitionConfidence.rejected ||
                 result.confidence == Windows.Media.SpeechRecognition.SpeechRecognitionConfidence.low) {
            resultTextArea.innerText = "Sorry, didn't get that \n\nTry saying ->\nblue background\nred border\ngreen circle";
        }
    }

    function drawCanvas() {
        /// <summary>
        /// Uses the new colors to redraw the image in the app.
        /// </summary>
        shapeCanvas.fillStyle = borderColor;
        shapeCanvas.fillRect(0, 0, 200, 200); // Border
        shapeCanvas.fillStyle = backgroundColor;
        shapeCanvas.fillRect(20, 20, 160, 160); // Background
        shapeCanvas.fillStyle = circleColor;
        shapeCanvas.beginPath();
        shapeCanvas.arc(100, 100, 80, 0, Math.PI * 2, true); // Circle
        shapeCanvas.fill();
    }

    function getColor(colorString) {
        /// <summary>
        /// Creates a color object from the passed in string.
        /// </summary>
        /// <param name="colorString">The name of the color.</param>
        /// <returns>The hex value of the color.</returns>
        var newColor = "#FFFFFF";

        if (colorString in colorLookup) {
            newColor = colorLookup[colorString];
        }

        return newColor;
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
