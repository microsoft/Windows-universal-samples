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
#include "Scenario1_AddAccount.xaml.h"
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


// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario1_AddAccount::Scenario1_AddAccount() : rootPage(MainPage::Current)
{
    InitializeComponent();
    addAccountLaunched = false;
}

void Scenario1_AddAccount::AddAccount_Click(Object^ sender, RoutedEventArgs^ e) 
{
    rootPage->NotifyUser("Add account launched", NotifyType::StatusMessage);
    
    // only one instance of the UserDataAccountManager pane can be launched at once per thread
    if(addAccountLaunched == false)
    {
        addAccountLaunched = true;

        // Allow user to add email, contact, and appointment accounts
        create_task(UserDataAccounts::UserDataAccountManager::ShowAddAccountAsync(UserDataAccountContentKinds::Email | UserDataAccountContentKinds::Appointment | UserDataAccountContentKinds::Contact))
            .then([this](Platform::String^ userDataAccountId)
        {
            if (userDataAccountId->IsEmpty())
            {
                rootPage->NotifyUser("User cancelled or add account failed", NotifyType::StatusMessage);
                return create_task([] {});
            }
            else
            {
                return DisplayUserInformationAsync(userDataAccountId);
            }
        }).then([this]
        {
            addAccountLaunched = false;
        });
    }
}

task<void> Scenario1_AddAccount::DisplayUserInformationAsync(String^ userId)
{
    auto getStoreTask = create_task(UserDataAccounts::UserDataAccountManager::RequestStoreAsync(UserDataAccountStoreAccessType::AllAccountsReadOnly));

    return getStoreTask.then([this, userId](UserDataAccountStore^ store)
    {
        // If the store is null, that means all access to Contacts, Calendar and Email data 
        // has been revoked.
        if (store == nullptr)
        {
            rootPage->NotifyUser("Access to Contacts, Calendar and Email has been revoked", NotifyType::ErrorMessage);
            return create_task([] {});
        }

        return create_task(store->GetAccountAsync(userId)).then([this](UserDataAccount^ account)
        {
            if (account != nullptr)
            {
                rootPage->NotifyUser("Added user: " + account->UserDisplayName, NotifyType::StatusMessage);
            }
        });
    });
}

