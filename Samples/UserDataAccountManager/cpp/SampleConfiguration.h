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

#pragma once 
#include "pch.h"

namespace SDKTemplate
{
    value struct Scenario;

    partial ref class MainPage
    {
    internal:
        static property Platform::String^ FEATURE_NAME
        {
            Platform::String^ get()
            {
                return "User Data Account Manager";
            }
        }

        static property Platform::Array<Scenario>^ scenarios
        {
            Platform::Array<Scenario>^ get()
            {
                return scenariosInner;
            }
        }

        Concurrency::task<void> LoadDataAccountsAsync(Windows::UI::Xaml::Controls::ComboBox^ comboBox);

    private:
        static Platform::Array<Scenario>^ scenariosInner;
    };

    // Wrapper class for binding.
    [Windows::UI::Xaml::Data::Bindable]
    public ref class UserDataAccountViewModel sealed
    {
    public:
        UserDataAccountViewModel(Windows::ApplicationModel::UserDataAccounts::UserDataAccount^ account) :
            account(account)
        {
        }

        property Windows::ApplicationModel::UserDataAccounts::UserDataAccount^ Account
        {
            Windows::ApplicationModel::UserDataAccounts::UserDataAccount^ get()
            {
                return account;
            }
        }

        property Platform::String^ UserDisplayName
        {
            Platform::String^ get()
            {
                return account->UserDisplayName;
            }
        }

    private:
        Windows::ApplicationModel::UserDataAccounts::UserDataAccount^ account;
    };

    public value struct Scenario
    {
        Platform::String^ Title;
        Platform::String^ ClassName;
    };
}
