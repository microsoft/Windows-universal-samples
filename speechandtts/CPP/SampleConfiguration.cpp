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

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace  SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Synthesize Text", "SpeechAndTTS.Scenario_SynthesizeText" },
    { "Synthesize SSML", "SpeechAndTTS.Scenario_SynthesizeSSML" },
    { "Predefined Dictation Grammar", "SpeechAndTTS.Scenario_PredefinedDictationGrammar" },
    { "Predefined WebSearch Grammar", "SpeechAndTTS.Scenario_PredefinedWebSearchGrammar" },
    { "Custom List Constraint", "SpeechAndTTS.Scenario_ListConstraint" },
    { "Custom SRGS Constraint", "SpeechAndTTS.Scenario_SRGSConstraint" },
    { "Continuous Dictation", "SpeechAndTTS.Scenario_ContinuousDictation" },
    { "Continuous List Constraint", "SpeechAndTTS.Scenario_ContinuousRecognitionListGrammar" },
    { "Continuous SRGS Constraint", "SpeechAndTTS.Scenario_ContinuousRecognitionSRGSGrammar" },
};
