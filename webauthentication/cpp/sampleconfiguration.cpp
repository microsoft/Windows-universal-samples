// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Connect to Google Services", "WebAuthentication.Scenario1_oAuthGoogle" },
    { "Connect to Facebook Services", "WebAuthentication.Scenario2_oAuthFacebook" }
};
