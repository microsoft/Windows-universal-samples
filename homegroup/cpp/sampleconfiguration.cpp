// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Open the file picker at HomeGroup",    "SDKTemplate.HomeGroupPicker" },
    { "Search HomeGroup",                     "SDKTemplate.SearchHomeGroup" },
    { "Stream video from Homegroup",          "SDKTemplate.HomeGroupVideoStream" },
    { "Advanced search",                      "SDKTemplate.HomeGroupAdvancedSearch" }
};
