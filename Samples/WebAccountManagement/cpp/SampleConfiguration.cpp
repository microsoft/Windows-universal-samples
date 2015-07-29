// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

// Important Note for running this sample:
// The sample as-is will not be able to get tokens without having it's app manifest being 
// modified to use the App Identity of a registered Microsoft Store/registered AAD app. 
//
// See 'Related Topics' in the README.md for instructions on how to register an app.

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Default Account", "SDKTemplate.Scenario1_DefaultAccount" },
    { "Microsoft account", "SDKTemplate.Scenario2_Msa" },
    { "Azure Active Directory", "SDKTemplate.Scenario3_Aad" },
    { "Many Providers, One Signed In Account", "SDKTemplate.Scenario4_ManyProviders" },
    { "Many Providers, Many Signed In Accounts", "SDKTemplate.Scenario5_ManyProvidersManyAccounts" },
    { "Custom Provider", "SDKTemplate.Scenario6_CustomProvider" }
};
