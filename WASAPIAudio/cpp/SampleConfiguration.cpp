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

using namespace SDKSample;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Device Enumeration", "SDKSample.WASAPIAudio.Scenario1" },
    { "Audio Rendering with Hardware Offload", "SDKSample.WASAPIAudio.Scenario2" },
    { "Audio Rendering with Low Latency", "SDKSample.WASAPIAudio.Scenario3" },
    { "PCM Audio Capture", "SDKSample.WASAPIAudio.Scenario4" }
};
