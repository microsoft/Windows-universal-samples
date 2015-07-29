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
#include "Scenario2_ManageAccount.xaml.h"
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

Scenario2_ManageAccount::Scenario2_ManageAccount() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario2_ManageAccount::OnNavigatedTo(NavigationEventArgs^ e)
{
    rootPage->LoadDataAccountsAsync(AccountsListComboBox);

}

void Scenario2_ManageAccount::Manage_Click(Object^ sender, RoutedEventArgs^ e)
{
    auto selectedAccount = safe_cast<UserDataAccountViewModel^>(AccountsListComboBox->SelectedValue);
    if (selectedAccount != nullptr)
    {
        // only one instance of the UserDataAccountManager pane can be launched at once per thread
        if (isLaunched == false)
        {
            isLaunched = true;
            create_task(UserDataAccounts::UserDataAccountManager::ShowAccountSettingsAsync(selectedAccount->Account->Id)).then([this]()
            {
                // User may have deleted the account
                return rootPage->LoadDataAccountsAsync(AccountsListComboBox);
            }).then([this]()
            {
                isLaunched = false;
            });
        }
    }
}
