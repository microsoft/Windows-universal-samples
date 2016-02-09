// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Save credentials", "SDKTemplate.Scenario1_Save" },
    { "Manage credentials", "SDKTemplate.Scenario2_Manage" },
};

bool MainPage::InitializePasswordVaultInTheBackground()
{
    concurrency::create_async([]()
    {
        // any call to the password vault will load the vault.
        auto vault = ref new Windows::Security::Credentials::PasswordVault();
        vault->RetrieveAll();
    });
    return true;

}
