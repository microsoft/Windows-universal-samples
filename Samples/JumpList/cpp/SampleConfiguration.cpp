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
    { "Respond to being launched", "SDKTemplate.Scenario1_Launched" },
    { "Remove the jump list", "SDKTemplate.Scenario2_Remove" },
    { "Creating custom items", "SDKTemplate.Scenario3_CustomItems" },
    { "Change the system group", "SDKTemplate.Scenario4_ChangeSystemGroup" },
    { "Check if the platform is supported", "SDKTemplate.Scenario5_IsSupported" },
};

void MainPage::LaunchParam::set(Platform::String^ value)
{
    launchParam = value;
    if (value != nullptr && !value->IsEmpty())
    {
        if (ScenarioControl->SelectedIndex == 0)
        {
            ScenarioControl->SelectedIndex = -1;
        }

        ScenarioControl->SelectedIndex = 0;
    }
}