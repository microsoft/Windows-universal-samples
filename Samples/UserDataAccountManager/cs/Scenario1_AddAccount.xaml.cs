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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.ApplicationModel.UserDataAccounts;
using System;
using System.Threading;
using System.Threading.Tasks;
using System.Collections.Generic;
using Windows.ApplicationModel.Email;

namespace SDKTemplate
{
    public sealed partial class Scenario1_AddAccount : Page
    {
        private MainPage rootPage;
        private bool isLaunched;

        public Scenario1_AddAccount()
        {
            this.InitializeComponent();
            isLaunched = false;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private async void AddAccount_Click(object sender, RoutedEventArgs e)
        {
            // only one instance of the UserDataAccountManager pane can be launched at once per thread
            if (isLaunched == false)
            {
                isLaunched = true;

                // Allow user to add email, contacts and appointment providing accounts.
                String userDataAccountId = await UserDataAccountManager.ShowAddAccountAsync(UserDataAccountContentKinds.Email | UserDataAccountContentKinds.Appointment | UserDataAccountContentKinds.Contact);

                if (String.IsNullOrEmpty(userDataAccountId))
                {
                    rootPage.NotifyUser("User cancelled or add account failed", NotifyType.StatusMessage);
                }
                else
                {
                    await DisplayUserInformationAsync(userDataAccountId);
                }

                isLaunched = false;
            }
        }

        private async Task DisplayUserInformationAsync(String id)
        {

            // Requires the email, contacts, or appointments in the app's manifest
            UserDataAccountStore store = await UserDataAccountManager.RequestStoreAsync(UserDataAccountStoreAccessType.AllAccountsReadOnly);

            // If the store is null, that means all access to Contacts, Calendar and Email data 
            // has been revoked.
            if (store == null)
            {
                rootPage.NotifyUser("Access to Contacts, Calendar and Email has been revoked", NotifyType.ErrorMessage);
                return;
            }

            UserDataAccount account = await store.GetAccountAsync(id);
            rootPage.NotifyUser("Added account: " + account.UserDisplayName, NotifyType.StatusMessage);

        }

    }
}
