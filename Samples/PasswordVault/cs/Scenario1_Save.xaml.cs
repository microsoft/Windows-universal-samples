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

using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Security.Credentials;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Save : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario1_Save()
        {
            this.InitializeComponent();
        }

        private void SaveCredential()
        {
            var resource = InputResourceValue.Text;
            var userName = InputUserNameValue.Text;
            var password = InputPasswordValue.Password;

            if (resource  == "" || userName == "" || password == "")
            {
                rootPage.NotifyUser("All fields are required when saving a credential.", NotifyType.ErrorMessage);
            }
            else
            {
                // Add a credential to PasswordVault with provided resource, user name, and password.
                // Replaces any existing credential with the same resource and user name.
                var vault = new PasswordVault();
                var cred = new PasswordCredential(resource, userName, password);
                vault.Add(cred);

                rootPage.NotifyUser("Credential saved successfully. Resource: " + cred.Resource + " Username: " + cred.UserName + " Password: " + cred.Password, NotifyType.StatusMessage);
            }
        }
    }
}
