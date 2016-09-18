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
#include "Scenario7_B2B.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario7_B2B sealed
    {
    public:
        Scenario7_B2B();

    protected:
        void GetCustomerCollectionsId(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void GetCustomerPurchaseId(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

    private:
        MainPage^ rootPage = MainPage::Current;
        Windows::Services::Store::StoreContext^ storeContext;

        Concurrency::task<Platform::String^> GetTokenFromAzureOAuthAsync();
    };
}
