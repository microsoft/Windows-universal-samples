// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Message", "SDKTemplate.Scenario1_Message" },
    { "Message+Caption", "SDKTemplate.Scenario2_MessageCaption" },
    { "CredentialPickerOptions", "SDKTemplate.Scenario3_CredentialPickerOptions" }
};
