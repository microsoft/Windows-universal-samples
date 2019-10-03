//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario1-ExtractTextSegments.html", {
        ready: function (element, options) {
            document.getElementById("WordSegmentButton").addEventListener("click", showWordSegments, false);
            document.getElementById("SelectionSegmentButton").addEventListener("click", showSelectionSegments, false);
        }
    });

    function showWordSegments() {

        // Initialize and obtain input values
        var notifyText = "";

        // Obtain the input string value, check for non-emptiness
        var inputStringBox = document.getElementById("inputStringBox");
        var inputStringText = inputStringBox.value;
        if ("" === inputStringText) {
            notifyText = "Cannot compute word segments: input string is empty.";
            WinJS.log && WinJS.log(notifyText, "sample", "error");
            return;
        }

        // Obtain the language tag value, check for non-emptiness and that it is valid
        // Ex. Valid Values:
        //     "en-US" (English (United States))
        //     "fr-FR" (French (France))
        //     "de-DE" (German (Germany))
        //     "ja-JP" (Japanese (Japan))
        //     "ar-SA" (Arabic (Saudi Arabia))
        //     "zh-CN" (China (PRC))
        var languageTagBox = document.getElementById("languageTagBox");
        var languageTagText = languageTagBox.value;
        if ("" === languageTagText) {
            notifyText += "Language tag input is empty ... using generic-language segmentation rules.";
            languageTagText = "und";
        }
        else {
            if (!Windows.Globalization.Language.isWellFormed(languageTagText)) {
                notifyText = "Language tag is not well formed: \"" + languageTagText + "\"";
                WinJS.log && WinJS.log(notifyText, "sample", "error");
                return;
            }
        }

        // Notify that we are going to calculate word segments
        notifyText += "\n\nCalculating word segments ...\n";
        notifyText += "\nInput: \"" + inputStringText + "\"";
        notifyText += "\nLanguage Tag: \"" + languageTagText + "\"";

        // Construct the WordsSegmenter instance
        var segmenter = new Windows.Data.Text.WordsSegmenter(languageTagText);

        // Tokenize the word segments, iterate over them and add to notifyText
        var wordSegments = segmenter.getTokens(inputStringText);
        wordSegments.forEach(
            function (wordSegment) {
                notifyText += "\n\tWord Bound---->\"" + wordSegment.text + "\"";
                var alternateForms = wordSegment.alternateForms;
                alternateForms.forEach(
                    function (alternateForm) {
                        notifyText += "\n\t\tAlternate---->\"" + alternateForm.alternateText + "\"";
                    }
                );
            }
        );

        // send notifyText to the output pane
        WinJS.log && WinJS.log(notifyText, "sample", "status");
    }

    function showSelectionSegments() {

        // Initialize and obtain input values
        var notifyText = "";

        // Obtain the input string value, check for non-emptiness
        var inputStringBox = document.getElementById("inputStringBox");
        var inputStringText = inputStringBox.value;
        if ("" === inputStringText) {
            notifyText = "\nCannot compute selection segments: input string is empty.";
            WinJS.log && WinJS.log(notifyText, "sample", "error");
            return;
        }

        // Obtain the language tag value, check for non-emptiness and that it is valid
        // Ex. Valid Values:
        //     "en-US" (English (United States))
        //     "fr-FR" (French (France))
        //     "de-DE" (German (Germany))
        //     "ja-JP" (Japanese (Japan))
        //     "ar-SA" (Arabic (Saudi Arabia))
        //     "zh-CN" (China (PRC))
        var languageTagBox = document.getElementById("languageTagBox");
        var languageTagText = languageTagBox.value;
        if ("" === languageTagText) {
            notifyText += "\nLanguage tag input is empty ... using generic-language segmentation rules.";
            languageTagText = "und";
        }
        else {
            if (!Windows.Globalization.Language.isWellFormed(languageTagText)) {
                notifyText = "\nLanguage tag is not well formed: \"" + languageTagText + "\"";
                WinJS.log && WinJS.log(notifyText, "sample", "error");
                return;
            }
        }

        // Notify that we are going to calculate selection segments
        notifyText += "\n\nCalculating selection segments ...\n";
        notifyText += "\nInput: \"" + inputStringText + "\"";
        notifyText += "\nLanguage Tag: \"" + languageTagText + "\"";

        // Construct the SelectableWordsSegmenter instance
        var segmenter = new Windows.Data.Text.SelectableWordsSegmenter(languageTagText);

        // Tokenize the selection segments, iterate over them and add to notifyText
        var selectionSegments = segmenter.getTokens(inputStringText);
        selectionSegments.forEach(
            function (selectionSegment) {
                notifyText += "\n\tSelection Bound---->\"" + selectionSegment.text + "\"";
            }
        );

        // send notifyText to the output pane
        WinJS.log && WinJS.log(notifyText, "sample", "status");
    }
})();
