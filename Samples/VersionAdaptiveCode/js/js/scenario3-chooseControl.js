//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    WinJS.Namespace.define("SdkSample", {
        // SpeechSynthesisControl uses the speechSynthesis object to speak text.
        SpeechSynthesisControl: WinJS.Class.define(function (element, options) {
            this.element = element || document.createElement("div");
            this.element.winControl = this;
            this.options = options;
            WinJS.UI.setOptions(this, options);
        }, {
            speak: function (text) {
                var utterance = new SpeechSynthesisUtterance(text);
                speechSynthesis.speak(utterance);
            }
        }),

        // SpeechSynthesizerControl uses the SpeechSynthesizer class to speak text.
        // See the SpeechRecognitionAndSynthesis sample for more information.
        SpeechSynthesizerControl: WinJS.Class.define(function (element, options) {
            this.element = element || document.createElement("div");
            this.element.winControl = this;
            this.options = options;
            WinJS.UI.setOptions(this, options);
        }, {
            speak: function (text) {
                try {
                    var audio = new Audio();
                } catch (e) {
                    if (e.number == 0x80004001 | 0) { // E_NOTIMPL
                        // If media components are not installed, creating an Audio object will fail.
                        return;
                    }
                }
                var synth = new Windows.Media.SpeechSynthesis.SpeechSynthesizer();
                synth.synthesizeTextToStreamAsync(text).done(function (markersStream) {
                    var blob = MSApp.createBlobFromRandomAccessStream(markersStream.ContentType, markersStream);
                    audio.src = URL.createObjectURL(blob, { oneTimeOnly: true });
                    audio.play();
                });
            }
        })
    });
        
    var page = WinJS.UI.Pages.define("/html/scenario3-chooseControl.html", {
        ready: function (element, options) {
            speakButton.addEventListener("click", speakText);

            // If the speechSynthesis object is available, then create a control that uses it.
            // Otherwise, create a control that uses the older SpeechSynthesizer object.
            if (window.speechSynthesis) {
                speechGenerator.dataset.winControl = "SdkSample.SpeechSynthesisControl";
                WinJS.log && WinJS.log("Using speechSynthesis object", "samples", "status");
            } else {
                speechGenerator.dataset.winControl = "SdkSample.SpeechSynthesizerControl";
                WinJS.log && WinJS.log("Using SpeechSynthesizer object", "samples", "status");
            }
            return WinJS.UI.processAll();
        }
    });

    function speakText() {
        if (textToSpeak.value.length > 0) {
            speechGenerator.winControl.speak(textToSpeak.value);
        }
    }
})();
