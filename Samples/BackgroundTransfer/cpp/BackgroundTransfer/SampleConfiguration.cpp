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
    { "File Download", "SDKTemplate.Scenario1_Download" },
    { "File Upload", "SDKTemplate.Scenario2_Upload" },
    { "Completion Notifications", "SDKTemplate.Scenario3_Notifications" },
    { "Completion Groups", "SDKTemplate.Scenario4_CompletionGroups" },
    { "Random Access Downloads", "SDKTemplate.Scenario5_RandomAccess" },
    { "Recoverable Errors", "SDKTemplate.Scenario6_RecoverableErrors" },
    { "Download Reordering", "SDKTemplate.Scenario7_DownloadReordering" },
};
