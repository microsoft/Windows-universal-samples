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

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>  
{
    // The format here is the following:
    //     { "Description for the sample", "Fully qualified name for the class that implements the scenario" }
    { "Trial-mode", "SDKTemplate.Scenario1_TrialMode" },
    { "In-app purchase", "SDKTemplate.Scenario2_InAppPurchase" },
    { "Expiring product", "SDKTemplate.Scenario3_ExpiringProduct" },
    { "Consumable product", "SDKTemplate.Scenario4_ConsumableProduct" },
    { "Advanced consumable product", "SDKTemplate.Scenario5_AdvancedConsumableProduct" },
    { "Large catalog product", "SDKTemplate.Scenario6_LargeCatalogProduct" },
    { "App listing URI", "SDKTemplate.Scenario7_AppListingURI" },
    { "Receipt", "SDKTemplate.Scenario8_Receipt" },
    { "Business to Business", "SDKTemplate.Scenario9_B2B" }
}; 

Concurrency::task<void> SDKTemplate::ConfigureSimulatorAsync(Platform::String^ filename)
{
    return Concurrency::create_task(Windows::ApplicationModel::Package::Current->InstalledLocation->GetFileAsync("data\\" + filename))
        .then([](Windows::Storage::StorageFile^ proxyFile)
    {
        return Windows::ApplicationModel::Store::CurrentAppSimulator::ReloadSimulatorAsync(proxyFile);
    });
}

int64_t SDKTemplate::DaysUntil(Windows::Foundation::DateTime endDate)
{
    auto calendar = ref new Windows::Globalization::Calendar();
    calendar->SetToNow();
    return static_cast<int>((endDate.UniversalTime - calendar->GetDateTime().UniversalTime) / 864000000000);
}