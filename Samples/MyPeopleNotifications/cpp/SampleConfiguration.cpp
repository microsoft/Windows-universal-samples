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
    { "Setup", "SDKTemplate.Scenario1_Setup" },
    { "Basics", "SDKTemplate.Scenario2_Basic" },
    { "Remote assets", "SDKTemplate.Scenario3_RemoteAssets" },
    { "tel: identification", "SDKTemplate.Scenario4_TelIdentification" },
    { "remoteid: identification", "SDKTemplate.Scenario5_RemoteId" },
    { "Gifs", "SDKTemplate.Scenario6_Gifs" },
    { "Spritesheets", "SDKTemplate.Scenario7_Spritesheets" },
    { "Offset spritesheets", "SDKTemplate.Scenario8_OffsetSpritesheets" }
};
