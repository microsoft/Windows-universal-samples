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
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using Windows.ApplicationModel.UserDataAccounts;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "User Data Account Manager";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Add an account", ClassType=typeof(Scenario1_AddAccount)},
            new Scenario() { Title="Manage an account", ClassType=typeof(Scenario2_ManageAccount)},
            new Scenario() { Title="Fix an account", ClassType=typeof(Scenario3_FixAccount)},
        };

        // Fill the specified ComboBox with user data accounts.
        public async Task LoadDataAccountsAsync(ComboBox comboBox)
        {
            // Requires the email, contacts, or appointments capability in the app's manifest.
            UserDataAccountStore store = await UserDataAccountManager.RequestStoreAsync(UserDataAccountStoreAccessType.AllAccountsReadOnly);

            if (store != null)
            {
                IReadOnlyList<UserDataAccount> userDataAccounts = await store.FindAccountsAsync();
                comboBox.DataContext = new ObservableCollection<UserDataAccount>(userDataAccounts);

                if (userDataAccounts.Count > 0)
                {
                    comboBox.SelectedIndex = 0;
                }
            }
            else
            {
                // If the store is null, that means all access to Contacts, Calendar and Email data 
                // has been revoked.
                NotifyUser("Access to Contacts, Calendar and Email has been revoked", NotifyType.ErrorMessage);
            }
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
