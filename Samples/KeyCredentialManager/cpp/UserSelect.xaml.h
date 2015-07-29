// Copyright (c) Microsoft. All rights reserved.
#pragma once
#include "UserSelect.g.h"
#include "MainPage.xaml.h"
#include "Account.h"

namespace SDKTemplate
{
    /// <summary>
    /// TODO
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class UserSelect sealed
    {
    public:
        UserSelect();       

    internal:   
        property static Platform::Collections::Vector<Account^>^ m_accountList;
        property static Windows::UI::Xaml::Controls::ListView^ m_accountListView;   

    private:
        MainPage^ rootPage;

        void OnLoadedActions(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void Button_SelectUser_Click(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
        void Button_AddUser_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void SetUpAccounts();
    };
}