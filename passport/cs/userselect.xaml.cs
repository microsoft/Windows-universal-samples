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

using SDKTemplate;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Serialization;
using Windows.Security.Credentials;
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// The user select page, if users successfully log into the app they will be added to a saved set of users. 
    /// That set of users will populate a list of account tiles that can be chosen from for signing in on this page.
    /// 
    /// If there are no accounts in the history, it will skip this and go directly to the traditional username/password sign in page.
    /// 
    /// If there exists 1 or more accounts, users can select an account tile which will pass the account object to the sign in form.
    /// </summary>
    public partial class UserSelect : Page
    {
        private MainPage rootPage;
        private ListView accountListView;
        static public List<Account> accountList;

        public UserSelect()
        {
            this.InitializeComponent();
            accountListView = listView_UserTileList;
            accountListView.SelectionChanged += Button_SelectUser_Click;
            this.Loaded += OnLoadedActions;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        /// <summary>
        /// Function called when the current frame is loaded.
        /// 
        /// Calls SetUpAccounts
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnLoadedActions(object sender, RoutedEventArgs e)
        {
            SetUpAccounts();
        }

        /// <summary>
        /// 
        /// Function called when an account is selected in the list of accounts
        ///
        /// Navigates to the sign in form and passes the chosen account
        /// The sign in form will check if Microsoft Passport should be used or not
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Button_SelectUser_Click(object sender, RoutedEventArgs e)
        {
            if (((ListView)sender).SelectedValue != null)
            {
                Account account = (Account)((ListView)sender).SelectedValue;
                rootPage.NotifyUser("Account " + account.Name + " selected!", NotifyType.StatusMessage);
                this.Frame.Navigate(typeof(SignIn), account);
            }
        }

        /// <summary>
        /// Function called when the "+" button is clicked for adding a new account
        ///
        /// Just navigates to the default sign in form with nothing filled out
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Button_AddUser_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(SignIn));
        }

        /// <summary>
        /// Uses the AccountsHelper to load the list of accounts from a saved local app file,
        /// then checks to see if it was empty or not. If it is empty then just go to the sign in form.
        /// </summary>
        private async void SetUpAccounts()
        {
            accountList = await AccountsHelper.LoadAccountList();
            accountListView.ItemsSource = accountList;

            if (accountList.Count == 0)
            {
                this.Frame.Navigate(typeof(SignIn));
            }
        }
    }
}
