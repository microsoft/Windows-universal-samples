// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "UserSelect.xaml.h"
#include "Account.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Interop;

UserSelect::UserSelect() : rootPage(MainPage::Current)
{
    InitializeComponent();
    m_accountListView = listView_UserTileList;
    m_accountListView->SelectionChanged += ref new SelectionChangedEventHandler(this, &SDKTemplate::UserSelect::Button_SelectUser_Click);
    this->Loaded += ref new RoutedEventHandler(this, &SDKTemplate::UserSelect::OnLoadedActions);
}

/// <summary>
/// Function called when the current frame is loaded.
/// 
/// Calls SetUpAccounts
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void UserSelect::OnLoadedActions(Object^ sender, RoutedEventArgs^ e)
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
void UserSelect::Button_SelectUser_Click(Object^ sender, SelectionChangedEventArgs^ e)
{
    if (((ListView^)sender)->SelectedValue != nullptr)
    {
        Account^ account = ref new Account();
        Account^ selectedAccount = m_accountList->GetAt(((ListView^)sender)->SelectedIndex);

        account->Name = selectedAccount->Name;
        account->Email = selectedAccount->Email;
        account->UsesPassport = selectedAccount->UsesPassport;

        rootPage->NotifyUser("Account " + account->Name + " selected!", NotifyType::StatusMessage);
        TypeName signInType = { "SDKTemplate.SignIn", TypeKind::Custom };
        this->Frame->Navigate(signInType, account);
    }
}

/// <summary>
/// Function called when the "+" button is clicked for adding a new account
///
/// Just navigates to the default sign in form with nothing filled out
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void UserSelect::Button_AddUser_Click(Object^ sender, RoutedEventArgs^ e)
{
    TypeName signInType = { "SDKTemplate.SignIn", TypeKind::Custom };
    this->Frame->Navigate(signInType);
}

/// <summary>
/// Checks to see if the user list has been created yet. 
/// If it hasn't then create it and then just go to the sign in form.
/// Otherwise, load the list of users into the ListView.
/// </summary>
void UserSelect::SetUpAccounts()
{
    if (m_accountList == nullptr)
    {
        m_accountList = ref new Platform::Collections::Vector<Account^>();
    }
    else
    {
        Platform::Collections::Vector<String^>^ usernames = ref new Platform::Collections::Vector<String^>();

        for (unsigned int i = 0; i < m_accountList->Size; i++) 
        {
            usernames->Append(m_accountList->GetAt(i)->Email);
        }

        m_accountListView->ItemsSource = usernames;
    }

    if (m_accountList->Size == 0)
    {
        TypeName signInType = { "SDKTemplate.SignIn", TypeKind::Custom };
        this->Frame->Navigate(signInType);
    }
}