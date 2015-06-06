// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Use custom commands", "SDKTemplate.CustomCommand" },
    { "Use default close command", "SDKTemplate.DefaultCloseCommand" },
    { "Use completed callback", "SDKTemplate.CompletedCallback" },
    { "Use cancel command", "SDKTemplate.CancelCommand" }
};
