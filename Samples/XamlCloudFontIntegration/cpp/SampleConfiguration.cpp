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

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Using this sample app", "SDKTemplate.Scenario_SampleOverview" },
    { "Downloadable fonts overview", "SDKTemplate.Scenario_CloudFontOverview" },
    { "Document 1", "SDKTemplate.Scenario_Document1" },
    { "Document 2", "SDKTemplate.Scenario_Document2" },
    { "Document 3", "SDKTemplate.Scenario_Document3" }
};
