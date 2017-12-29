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
using SDKTemplate;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Speech Recognition\nand TTS";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Synthesize Text", ClassType=typeof(Scenario_SynthesizeText)},
            new Scenario() { Title="Synthesize Text with Boundaries", ClassType=typeof(Scenario_SynthesizeTextBoundaries)},
            new Scenario() { Title="Synthesize SSML", ClassType=typeof(Scenario_SynthesizeSSML)},
            new Scenario() { Title="Predefined Dictation Grammar", ClassType=typeof(Scenario_PredefinedDictationGrammar)},
            new Scenario() { Title="Predefined WebSearch Grammar", ClassType=typeof(Scenario_PredefinedWebSearchGrammar)},
            new Scenario() { Title="Custom List Constraint", ClassType=typeof(Scenario_ListConstraint)},
            new Scenario() { Title="Custom SRGS Constraint", ClassType=typeof(Scenario_SRGSConstraint)},
            new Scenario() { Title="Continuous Dictation", ClassType=typeof(Scenario_ContinuousDictation)},
            new Scenario() { Title="Continuous List Commands", ClassType=typeof(Scenario_ContinuousRecognitionListGrammar)},
            new Scenario() { Title="Continuous SRGS Commands", ClassType=typeof(Scenario_ContinuousRecognitionSRGSGrammar)},
            new Scenario() { Title="PauseAsync to Change Grammar", ClassType=typeof(Scenario_PauseAsync)}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
