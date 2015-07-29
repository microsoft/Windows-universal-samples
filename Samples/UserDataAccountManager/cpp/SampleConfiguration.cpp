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
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;
using namespace Concurrency;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::UserDataAccounts;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml::Controls;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Add an account", "SDKTemplate.Scenario1_AddAccount" },
    { "Manage an account", "SDKTemplate.Scenario2_ManageAccount" },
    { "Resolve errors for an account", "SDKTemplate.Scenario3_FixAccount" }
};

task<void> MainPage::LoadDataAccountsAsync(ComboBox^ comboBox)
{
    // Requires the email, contacts, or appointments capability in the app's manifest.
    return create_task(UserDataAccountManager::RequestStoreAsync(UserDataAccountStoreAccessType::AllAccountsReadOnly))
        .then([this, comboBox](UserDataAccountStore^ store)
    {
        if (store != nullptr)
        {
            return create_task(store->FindAccountsAsync()).then([this, comboBox](IVectorView<UserDataAccount^>^ accounts)
            {
                auto userDataAccounts = ref new Vector<UserDataAccountViewModel^>();
                for (UserDataAccount^ account : accounts)
                {
                    userDataAccounts->Append(ref new UserDataAccountViewModel(account));
                }

                comboBox->DataContext = userDataAccounts;

                if (userDataAccounts->Size > 0)
                {
                    comboBox->SelectedIndex = 0;
                }
            });
        }
        else
        {
            // If the store is null, that means all access to Contacts, Calendar and Email data 
            // has been revoked.
            NotifyUser("Access to Contacts, Calendar and Email has been revoked", NotifyType::ErrorMessage);
            return create_task([] {});
        }
    });
}