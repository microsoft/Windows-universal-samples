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
    { "Scenario 1", "SDKTemplate.Scenario1" },
    { "Scenario 2", "SDKTemplate.Scenario2" },
    { "Scenario 3", "SDKTemplate.Scenario3" },
    { "Scenario 4", "SDKTemplate.Scenario4" },
    { "Scenario 5", "SDKTemplate.Scenario5" }
};
