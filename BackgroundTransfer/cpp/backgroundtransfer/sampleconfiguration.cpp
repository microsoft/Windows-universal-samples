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
    { "File Download", "BackgroundTransfer.Scenario1_Download" },
    { "File Upload", "BackgroundTransfer.Scenario2_Upload" },
    { "Completion Notifications", "BackgroundTransfer.Scenario3_Notifications" },
    { "Completion Groups", "BackgroundTransfer.Scenario4_CompletionGroups" }
};
