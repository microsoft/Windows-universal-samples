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

using Windows.ApplicationModel.UserDataAccounts;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using System;
using System.Collections.Generic;
using Windows.UI.Xaml;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Threading.Tasks;

namespace SDKTemplate
{
    public sealed partial class Scenario2_ManageAccount : Page
    {
        private MainPage rootPage = MainPage.Current;
        private bool isLaunched = false;

        public Scenario2_ManageAccount()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            await rootPage.LoadDataAccountsAsync(AccountsListComboBox);
        }

        private async void Manage_Click(object sender, RoutedEventArgs e)
        {
            // only one instance of the UserDataAccountManager pane can be launched at once per thread
            if (isLaunched == false)
            {
                isLaunched = true;

                var selectedAccount = (UserDataAccount)AccountsListComboBox.SelectedValue;
                if (selectedAccount != null)
                {
                    await UserDataAccountManager.ShowAccountSettingsAsync(selectedAccount.Id);

                    // User may have deleted the account
                    await rootPage.LoadDataAccountsAsync(AccountsListComboBox);
                }

                isLaunched = false;
            }
        }
    }
}

