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
    { "Trimming and Saving a clip", "SDKTemplate.Scenario1_TrimAndSaveClip" },
    { "Appending multiple clips", "SDKTemplate.Scenario2_AppendClips" },
    { "Adding background audio tracks", "SDKTemplate.Scenario3_AddAudioTracks" },
    { "Adding overlays to a clip", "SDKTemplate.Scenario4_AddOverlays" }
};
