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
using Windows.ApplicationModel.Email;
using Windows.ApplicationModel.Appointments;
using Windows.ApplicationModel.Contacts;
using System.Threading.Tasks;
using System.Collections.ObjectModel;

namespace SDKTemplate
{

    public sealed partial class Scenario3_FixAccount : Page
    {
        private MainPage rootPage = MainPage.Current;
        private bool isLaunched = false;

        public Scenario3_FixAccount()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            await rootPage.LoadDataAccountsAsync(AccountsListComboBox);
        }

        private async void Fix_Click(object sender, RoutedEventArgs e)
        {
            // only one instance of the UserDataAccountManager pane can be launched at once per thread
            if (isLaunched == false)
            {
                isLaunched = true;
                rootPage.NotifyUser("", NotifyType.StatusMessage);

                var selectedAccount = (UserDataAccount)AccountsListComboBox.SelectedValue;
                if (selectedAccount != null)
                {
                    if (await IsFixNeededAsync(selectedAccount))
                    {
                        await UserDataAccountManager.ShowAccountErrorResolverAsync(selectedAccount.Id);
                    }
                    else
                    {
                        rootPage.NotifyUser("Account is not in an error state", NotifyType.ErrorMessage);
                    }

                    await rootPage.LoadDataAccountsAsync(AccountsListComboBox);
                }

                isLaunched = false;
            }
        }

        /// <summary>
        /// Check the mailboxes, calendars, and contact lists for sync errors
        /// </summary>
        /// <param name="selectedAccount"></param>
        /// <returns>True if there is an sync error for email, calendar, or contacts. False if nothing is syncing or everything is syncing correctly</returns>
        private async Task<bool> IsFixNeededAsync(UserDataAccount selectedAccount)
        {
            foreach (EmailMailbox mailbox in await selectedAccount.FindEmailMailboxesAsync())
            {
                if (mailbox.SyncManager != null)
                {
                    switch (mailbox.SyncManager.Status)
                    {
                        case EmailMailboxSyncStatus.AuthenticationError:
                        case EmailMailboxSyncStatus.PolicyError:
                        case EmailMailboxSyncStatus.UnknownError:
                            return true;
                    }
                }
            }

            foreach (AppointmentCalendar calendar in await selectedAccount.FindAppointmentCalendarsAsync())
            {
                if (calendar.SyncManager != null)
                {
                    switch (calendar.SyncManager.Status)
                    {
                        case AppointmentCalendarSyncStatus.AuthenticationError:
                        case AppointmentCalendarSyncStatus.PolicyError:
                        case AppointmentCalendarSyncStatus.UnknownError:
                            return true;
                    }
                }
            }

            foreach (ContactList contactList in await selectedAccount.FindContactListsAsync())
            {
                if (contactList.SyncManager != null)
                {
                    switch (contactList.SyncManager.Status)
                    {
                        case ContactListSyncStatus.AuthenticationError:
                        case ContactListSyncStatus.PolicyError:
                        case ContactListSyncStatus.UnknownError:
                            return true;
                    }
                }
            }

            return false;
        }
    }
}
