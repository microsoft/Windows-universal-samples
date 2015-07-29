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
    { "AppBarButtons", "SDKTemplate.Scenario1_AppBarButtons" },
    { "Icons", "SDKTemplate.Scenario2_Icons" },
    { "Opening/Closing Events", "SDKTemplate.Scenario3_Events" },
    { "Control the CommandBar", "SDKTemplate.Scenario4_Control" },
    { "Multiple CommandBars", "SDKTemplate.Scenario5_Multiple" },
    { "Styling", "SDKTemplate.Scenario6_Styling" },
    { "Custom Content", "SDKTemplate.Scenario7_CustomContent" },
    { "Command Reflow", "SDKTemplate.Scenario8_Reflow" }
};
