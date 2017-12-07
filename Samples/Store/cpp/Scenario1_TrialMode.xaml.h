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
#include "Scenario1_TrialMode.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_TrialMode sealed
    {
    public:
        Scenario1_TrialMode();
        void ShowTrialPeriodInformation();
        void PurchaseFullLicense();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e) override;
    private:
        MainPage^ rootPage = MainPage::Current;
        Windows::Services::Store::StoreContext^ storeContext;
        Windows::Foundation::EventRegistrationToken eventRegistrationToken;

        void OfflineLicensesChanged(Windows::Services::Store::StoreContext^ sender, Platform::Object^ args);
        void GetLicenseState();
    };
}

