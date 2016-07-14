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
#include "Scenario9_B2B.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario9_B2B sealed
    {
    public:
        Scenario9_B2B();
        void GetCustomerCollectionsId();
        void GetCustomerPurchaseId();

    private:
        Concurrency::task<Platform::String^> GetTokenFromAzureOAuthAsync();
        void DisplayResults(Platform::String^ results);

    private:
        MainPage^ rootPage = MainPage::Current;
    };
}
