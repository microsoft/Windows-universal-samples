// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Read and write simple structured data.", "SDKTemplate.WriteReadStream" },
    { "Dump file contents using ReadBytes().", "SDKTemplate.ReadBytes" }
};
