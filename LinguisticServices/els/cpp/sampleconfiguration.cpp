// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Language Detection", "SDKTemplate.Scenario1_LanguageDetection" },
    { "Script Detection", "SDKTemplate.Scenario2_ScriptDetection" },
    { "Transliteration", "SDKTemplate.Scenario3_Transliteration" }
};
