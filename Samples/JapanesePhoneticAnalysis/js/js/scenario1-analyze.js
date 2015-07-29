//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario1-analyze.html", {
        ready: function (element, options) {
            getPhrasesButton.addEventListener("click", getPhrases);
        }
    });

    function getPhrases() {
        var input = inputText.value;
        var output = "";

        // monoRuby = false means that each element in the result corresponds to a single Japanese word.
        // monoRuby = true means that each element in the result corresponds to one or more characters which are pronounced as a single unit.
        var monoRuby = monoRubyRadioButton.checked;

        // Analyze the Japanese text according to the specified algorithm.
        var words = Windows.Globalization.JapanesePhoneticAnalyzer.getWords(input, monoRuby);
        words.forEach(function(word) {
            // Put each phrase on its own line.
            if (output !== "" && word.isPhraseStart) {
                output += "\n";
            }
            // DisplayText is the display text of the word, which has same characters as the input of GetWords().
            // YomiText is the reading text of the word, as known as Yomi, which typically consists of Hiragana characters.
            // However, please note that the reading can contains some non-Hiragana characters for some display texts such as emoticons or symbols.
            output += word.displayText + "(" + word.yomiText + ")";
        });

        // Display the result.
        outputText.innerText = output;

        if (input !== "" && words.length == 0)
        {
            // If the result from GetWords() is empty but the input is not empty,
            // it means the given input is too long to analyze.
            WinJS.log && WinJS.log("Failed to get words from the input text.  The input text is too long to analyze.", "sample", "error");
        }
        else
        {
            // Otherwise, the analysis has been done successfully.
            WinJS.log && WinJS.log("Got words from the input text successfully.", "sample", "status");
        }
    }
})();
