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
    // The format here is the following:
    //     { "Description for the sample", "Fully qualified name for the class that implements the scenario" }
    { "Start Datagram Listener", "SDKTemplate.DatagramSocketSample.Scenario1" }, 
    { "Connect to Listener", "SDKTemplate.DatagramSocketSample.Scenario2" },
    { "Send Data", "SDKTemplate.DatagramSocketSample.Scenario3" },
    { "Close Socket", "SDKTemplate.DatagramSocketSample.Scenario4" }
}; 
