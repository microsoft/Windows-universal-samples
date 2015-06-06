// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
	{ "Default Account", "SDKTemplate.Scenario1_DefaultAccount" },
    { "MSA Provider", "SDKTemplate.Scenario2_Msa" },
    { "AAD Provider", "SDKTemplate.Scenario3_Aad" },
    { "Many Providers, One Signed In Account", "SDKTemplate.Scenario4_ManyProviders" },
    { "Many Providers, Many Signed In Accounts", "SDKTemplate.Scenario5_ManyProvidersManyAccounts" },
    { "Custom Provider", "SDKTemplate.Scenario6_CustomProvider" }
};
