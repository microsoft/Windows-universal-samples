// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    // The format here is the following:
    { "Using Custom Components",                                     "SDKSample.ProxyStubsForWinRTComponents.OvenClient" },
    { "Using Custom Components (Implemented using WRL)",             "SDKSample.ProxyStubsForWinRTComponents.OvenClientWRL" },
    { "Handling Windows Runtime Exceptions",                         "SDKSample.ProxyStubsForWinRTComponents.CustomException" },
    { "Handling Windows Runtime Exceptions (Implemented using WRL)", "SDKSample.ProxyStubsForWinRTComponents.CustomExceptionWRL" }
};
