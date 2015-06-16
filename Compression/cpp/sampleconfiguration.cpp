// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "WinRT to WinRT compression/decompression", "SDKTemplate.Compression.Scenario1" },
    { "WinRT to Classic compression/decompression", "SDKTemplate.Compression.Scenario2" },
    { "Classic to WinRT compression/decompression", "SDKTemplate.Compression.Scenario3" }
};
