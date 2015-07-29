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
    var page = WinJS.UI.Pages.define("/html/scenario6_SRGSConstraint.html", {
        ready: function (element, options) {
            AudioCapturePermissions.requestMicrophonePermission().then(function (available) {
                if (available) {
                    var c = document.getElementById("shapes");
                    shapeCanvas = c.getContext("2d");

                    initializeRecognizer();
                    drawCanvas();

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
    var shapeCanvas;
    var borderColor = "#FFFFFF";
    var backgroundColor = "#808080";
    var circleColor = "#000000";
    var colorLookup = {};
    colorLookup["red"] = "#FF0000";
    colorLookup["blue"] = "#0000FF";
    colorLookup["black"] = "#000000";
    colorLookup["brown"] = "#A52A2A";
    colorLookup["purple"] = "#800080";
    colorLookup["green"] = "#008000";
    colorLookup["yellow"] = "#FFFF00";
    colorLookup["cyan"] = "#00FFFF";
    colorLookup["magenta"] = "#FF00FF";
    colorLookup["orange"] = "#FFA500";
    colorLookup["gray"] = "#808080";
    colorLookup["white"] = "#FFFFFF";

    function initializeRecognizer() {
        /// <summary>
        /// Initialize speech recognizer and compile constraints.
        /// </summary>
        /// <remarks>
        /// For more information about grammars for Windows apps and how to
        /// define and use SRGS-compliant grammars in your app, see
        /// https://msdn.microsoft.com/en-us/library/dn596121.aspx
        /// </remarks>
        recognizer = Windows.Media.SpeechRecognition.SpeechRecognizer();

        // Provide feedback to the user about the state of the recognizer.
        recognizer.addEventListener('statechanged', onSpeechRecognizerStateChanged, false);

        // Specify the location of your grammar file, then retrieve it as a StorageFile.
        var grammarFilePath = "ms-appx:///SRGSColors.xml";

        Windows.Storage.StorageFile.getFileFromApplicationUriAsync(new Windows.Foundation.Uri(grammarFilePath)).done(
            function (result) {
                var fileConstraint = new Windows.Media.SpeechRecognition.SpeechRecognitionGrammarFileConstraint(result, "colors");
                recognizer.constraints.append(fileConstraint);

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
                        handleRecognitionResult(result);
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
                        handleRecognitionResult(result);
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
            if ("background" in semanticsObj.properties && semanticsObj.properties["background"].toString() != "...") {
                var newBackgroundColor = semanticsObj.properties["background"].toString();
                backgroundColor = getColor(newBackgroundColor.toLowerCase());
            }

            // If "background" was matched, but the color rule matched GARBAGE, prompt the user.
            else if ("background" in semanticsObj.properties && semanticsObj.properties["background"].toString() == "...") {

                garbagePrompt += "Didn't get the background color \n\nTry saying blue background\n";
                resultTextArea.innerText = garbagePrompt;
            }

            // BORDER: Check to see if the recognition result contains the semantic key for the border color,
            // and not a match for the GARBAGE rule, and change the color.
            if ("border" in semanticsObj.properties && semanticsObj.properties["border"].toString() != "...") {
                var newBorderColor = semanticsObj.properties["border"].toString();
                borderColor = getColor(newBorderColor.toLowerCase());
            }

            // If "border" was matched, but the color rule matched GARBAGE, prompt the user.
            else if ("border" in semanticsObj.properties && semanticsObj.properties["border"].toString() == "...") {
                garbagePrompt += "Didn't get the border color\n\n Try saying red border\n";
                resultTextArea.innerText = garbagePrompt;
            }

            // CIRCLE: Check to see if the recognition result contains the semantic key for the circle color,
            // and not a match for the GARBAGE rule, and change the color.
            if ("circle" in semanticsObj.properties && semanticsObj.properties["circle"].toString() != "...") {
                var newCircleColor = semanticsObj.properties["circle"].toString();
                circleColor = getColor(newCircleColor.toLowerCase());
            }

            // If "circle" was matched, but the color rule matched GARBAGE, prompt the user.
            else if ("circle" in semanticsObj.properties && semanticsObj.properties["circle"].toString() == "...") {
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

        if (colorString.toLowerCase() in colorLookup) {
            newColor = colorLookup[colorString.toLowerCase()];
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
