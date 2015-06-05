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

using namespace Commanding;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "AppBarButtons", "Commanding.Scenario1_AppBarButtons" },
    { "Icons", "Commanding.Scenario2_Icons" },
    { "Opening/Closing Events", "Commanding.Scenario3_Events" },
    { "Control the CommandBar", "Commanding.Scenario4_Control" },
    { "Multiple CommandBars", "Commanding.Scenario5_Multiple" },
    { "Styling", "Commanding.Scenario6_Styling" },
    { "Custom Content", "Commanding.Scenario7_CustomContent" },
    { "Command Reflow", "Commanding.Scenario8_Reflow" }
};
