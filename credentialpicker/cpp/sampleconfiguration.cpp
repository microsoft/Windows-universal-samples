// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Message", "CredentialPicker.Scenario1_Message" },
    { "Message+Caption", "CredentialPicker.Scenario2_MessageCaption" },
    { "CredentialPickerOptions", "CredentialPicker.Scenario3_CredentialPickerOptions" }
};
