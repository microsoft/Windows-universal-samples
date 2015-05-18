// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Show a context menu",            "SDKTemplate.Scenario1" },
    { "Replace a default context menu", "SDKTemplate.Scenario2" },
};
