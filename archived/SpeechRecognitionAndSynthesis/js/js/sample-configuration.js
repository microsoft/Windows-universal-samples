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

    var sampleTitle = "Speech Recognition \nand Text To Speech";

    var scenarios = [
        { url: "/html/scenario1_SynthesizeText.html", title: "Synthesize Text" },
        { url: "/html/scenario2_SynthesizeTextBoundaries.html", title: "Synthesize Text with Boundaries" },
        { url: "/html/scenario3_SynthesizeSSML.html", title: "Synthesize SSML" },
        { url: "/html/scenario4_PredefinedDictationGrammar.html", title: "Predefined Dictation Grammar" },
        { url: "/html/scenario5_PredefinedWebSearchGrammar.html", title: "Predefined Web Search Grammar" },
        { url: "/html/scenario6_ListConstraint.html", title: "Custom List Constraint" },
        { url: "/html/scenario7_SRGSConstraint.html", title: "Custom SRGS Constraint" },
        { url: "/html/scenario8_ContinuousDictation.html", title: "Continuous Dictation" },
        { url: "/html/scenario9_ContinuousRecognitionListGrammar.html", title: "Continuous List Commands" },
        { url: "/html/scenario10_ContinuousRecognitionSRGSGrammar.html", title: "Continuous SRGS Commands" },
        { url: "/html/scenario11_PauseAsync.html", title: "PauseAsync to Change Grammar" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();