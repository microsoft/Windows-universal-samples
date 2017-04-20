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
#include "Scenario6_AppListingURI.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario6_AppListingURI sealed
    {
    public:
        Scenario6_AppListingURI();

    protected:
        void DisplayLink(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

    private:
        MainPage^ rootPage = MainPage::Current;
        Windows::Services::Store::StoreContext^ storeContext;
    };
}
