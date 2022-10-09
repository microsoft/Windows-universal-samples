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
#include "Scenario1_Save.h"
#include "Scenario1_Save.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::Security::Credentials;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_Save::Scenario1_Save()
    {
        InitializeComponent();
    }

    void Scenario1_Save::SaveCredential_Click(IInspectable const&, IInspectable const&)
    {
        auto resource = InputResourceValue().Text();
        auto userName = InputUserNameValue().Text();
        auto password = InputPasswordValue().Password();

        if (resource.empty() || userName.empty() || password.empty())
        {
            rootPage.NotifyUser(L"All fields are required when saving a credential.", NotifyType::ErrorMessage);
        }
        else
        {
            // Add a credential to PasswordVault with provided resource, user name, and password.
            // Replaces any existing credential with the same resource and user name.
            PasswordVault vault;
            PasswordCredential cred{ resource, userName, password };
            vault.Add(cred);

            rootPage.NotifyUser(L"Credential saved successfully. Resource: " + cred.Resource() + L" Username: " + cred.UserName() + L" Password: " + cred.Password(), NotifyType::StatusMessage);
        }
    }
}

