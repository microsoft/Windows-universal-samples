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
    { "Connect to an NT Service",                 "SDKTemplate.MeteringData" },
    { "Connect to the FX2 Device",                "SDKTemplate.DeviceConnect" },
    { "Send IOCTLs to the device",                "SDKTemplate.DeviceIO" },
    { "Handle asynchronous device events",        "SDKTemplate.DeviceEvents" },
    { "Read and Write operations",                "SDKTemplate.DeviceReadWrite" }
};
