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
#include "Scenario1_Save.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Security::Credentials;

Scenario1_Save::Scenario1_Save()
{
    InitializeComponent();
}

void Scenario1_Save::SaveCredential()
{
    auto resource = InputResourceValue->Text;
    auto username = InputUserNameValue->Text;
    auto password = InputPasswordValue->Password;

    if (!resource || !username || !password)
    {
        MainPage::Current->NotifyUser("All fields are required when saving a credential.", NotifyType::ErrorMessage);
    }
    else
    {
        // Add a credential to PasswordVault with provided resource, user name, and password.
        // Replaces any existing credential with the same resource and user name.
        auto vault = ref new PasswordVault();
        auto cred = ref new PasswordCredential(resource, username, password);
        vault->Add(cred);

        rootPage->NotifyUser("Credential saved successfully. Resource: " + cred->Resource + " Username: " + cred->UserName + " Password: " + cred->Password, NotifyType::StatusMessage);
    }
}
