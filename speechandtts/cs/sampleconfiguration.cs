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

using System;
using System.Collections.Generic;
using Windows.UI.Xaml.Controls;
using SpeechAndTTS;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Speech and TTS";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Synthesize Text", ClassType=typeof(SynthesizeTextScenario)},
            new Scenario() { Title="Synthesize SSML", ClassType=typeof(SynthesizeSSMLScenario)},
            new Scenario() { Title="Predefined Dictation Grammar", ClassType=typeof(PredefinedDictationGrammarScenario)},
            new Scenario() { Title="Predefined WebSearch Grammar", ClassType=typeof(PredefinedWebSearchGrammarScenario)},
            new Scenario() { Title="Custom List Constraint", ClassType=typeof(ListConstraintScenario)},
            new Scenario() { Title="Custom SRGS Constraint", ClassType=typeof(SRGSConstraintScenario)},
            new Scenario() { Title="Continuous Dictation", ClassType=typeof(ContinuousDictationScenario)},
            new Scenario() { Title="Continuous List Commands", ClassType=typeof(ContinuousRecoListGrammarScenario)},
            new Scenario() { Title="Continuous SRGS Commands", ClassType=typeof(ContinuousRecoSRGSConstraintScenario)}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
