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
    { "Synthesize Text", "SDKTemplate.Scenario_SynthesizeText" },
    { "Synthesize SSML", "SDKTemplate.Scenario_SynthesizeSSML" },
    { "Predefined Dictation Grammar", "SDKTemplate.Scenario_PredefinedDictationGrammar" },
    { "Predefined WebSearch Grammar", "SDKTemplate.Scenario_PredefinedWebSearchGrammar" },
    { "Custom List Constraint", "SDKTemplate.Scenario_ListConstraint" },
    { "Custom SRGS Constraint", "SDKTemplate.Scenario_SRGSConstraint" },
    { "Continuous Dictation", "SDKTemplate.Scenario_ContinuousDictation" },
    { "Continuous List Constraint", "SDKTemplate.Scenario_ContinuousRecognitionListGrammar" },
    { "Continuous SRGS Constraint", "SDKTemplate.Scenario_ContinuousRecognitionSRGSGrammar" },
    { "PauseAsync to Change Grammars", "SDKTemplate.Scenario_PauseAsync" }
};
