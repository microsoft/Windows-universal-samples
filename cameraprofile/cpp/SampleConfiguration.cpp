//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
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
    { "Locate Record Specific Profile", "SDKTemplate.Scenario1_SetRecordProfile" },
    { "Query Profile for Concurrency", "SDKTemplate.Scenario2_ConcurrentProfile" },
    { "Query Profile for HDR Support", "SDKTemplate.Scenario3_EnableHdrProfile" }
};
