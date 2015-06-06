// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Get display name", "SDKTemplate.GetUserDisplayName" },
    { "Get first and last name", "SDKTemplate.GetUserFirstAndLastName" },
    { "Get account picture", "SDKTemplate.GetAccountPicture" },
    { "Set account picture and listen for changes", "SDKTemplate.SetAccountPicture" }
};
