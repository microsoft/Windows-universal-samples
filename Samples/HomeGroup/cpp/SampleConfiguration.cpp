// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Open the file picker at HomeGroup",    "SDKTemplate.Scenario1_Pick" },
    { "Search HomeGroup",                     "SDKTemplate.Scenario2_Search" },
    { "Stream video from HomeGroup",          "SDKTemplate.Scenario3_StreamVideo" },
    { "Search a HomeGroup user",              "SDKTemplate.Scenario4_SearchUser" }
};
