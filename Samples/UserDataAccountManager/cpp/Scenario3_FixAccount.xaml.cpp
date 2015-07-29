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
#include "Scenario3_FixAccount.xaml.h"

#include <ppltasks.h>

using namespace SDKTemplate;
using namespace concurrency;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::UserDataAccounts;

Scenario3_FixAccount::Scenario3_FixAccount() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario3_FixAccount::OnNavigatedTo(NavigationEventArgs^ e)
{
    rootPage->LoadDataAccountsAsync(AccountsListComboBox);
}

void Scenario3_FixAccount::Fix_Click(Object^ sender, RoutedEventArgs^ e)
{
    auto selectedAccount = safe_cast<UserDataAccountViewModel^>(AccountsListComboBox->SelectedValue);
    if (selectedAccount != nullptr)
    {
        // only one instance of the UserDataAccountManager pane can be launched at once per thread
        if (isLaunched == false)
        {
            isLaunched = true;
            rootPage->NotifyUser("", NotifyType::StatusMessage);

            auto account = selectedAccount->Account;
            IsFixNeededAsync(account).then([this, account](bool result)
            {
                if (result)
                {
                    return create_task(UserDataAccountManager::ShowAccountErrorResolverAsync(account->Id));
                }
                else
                {
                    rootPage->NotifyUser("Account is not in an error state", NotifyType::ErrorMessage);
                    return create_task([] {});
                }

            }).then([this]()
            {
                return rootPage->LoadDataAccountsAsync(AccountsListComboBox);
            }).then([this]()
            {
                isLaunched = false;
            });
        }
    }
}

/// Check the mailboxes, calendars, and contact lists for sync errors

Concurrency::task<bool> Scenario3_FixAccount::IsFixNeededAsync(UserDataAccount^ account)
{
    return create_task(account->FindEmailMailboxesAsync()).then([this, account](IVectorView<Email::EmailMailbox^>^ mailboxes)
    {
        for (Email::EmailMailbox^ mailbox : mailboxes)
        {
            if (mailbox->SyncManager != nullptr)
            {
                switch (mailbox->SyncManager->Status)
                {
                case Email::EmailMailboxSyncStatus::AuthenticationError:
                case Email::EmailMailboxSyncStatus::PolicyError:
                case Email::EmailMailboxSyncStatus::UnknownError:
                    return true;
                }
            }
        }
        return false;
    }).then([this, account](bool isFixNeeded)
    {
        if (isFixNeeded)
        {
            return create_task([] { return true; });
        }
        else
        {
            return create_task(account->FindAppointmentCalendarsAsync()).then([this, account](IVectorView<Appointments::AppointmentCalendar^>^ calendars)
            {
                for (Appointments::AppointmentCalendar^ calendar : calendars)
                {
                    if (calendar->SyncManager != nullptr)
                    {
                        switch (calendar->SyncManager->Status)
                        {
                        case Appointments::AppointmentCalendarSyncStatus::AuthenticationError:
                        case Appointments::AppointmentCalendarSyncStatus::PolicyError:
                        case Appointments::AppointmentCalendarSyncStatus::UnknownError:
                            return true;
                        }
                    }
                }
                return false;
            });
        }
    }).then([this, account](bool isFixNeeded)
    {
        if (isFixNeeded)
        {
            return create_task([] { return true; });
        }
        else
        {
            return create_task(account->FindContactListsAsync()).then([this, account](IVectorView<Contacts::ContactList^>^ contacts)
            {
                for (Contacts::ContactList^ contactList : contacts)
                {
                    if (contactList->SyncManager != nullptr)
                    {
                        switch (contactList->SyncManager->Status)
                        {
                        case Contacts::ContactListSyncStatus::AuthenticationError:
                        case Contacts::ContactListSyncStatus::PolicyError:
                        case Contacts::ContactListSyncStatus::UnknownError:
                            return true;
                        }
                    }
                }
                return false;
            });
        }
    });
}
