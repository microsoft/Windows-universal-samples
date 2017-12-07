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
#include "ItemsListViewModel.h"

using namespace SDKTemplate;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Services::Store;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>  
{
    // The format here is the following:
    //     { "Description for the sample", "Fully qualified name for the class that implements the scenario" }
    { "Trial-mode", "SDKTemplate.Scenario1_TrialMode" },
    { "In-app purchase", "SDKTemplate.Scenario2_InAppPurchase" },
    { "Unmanaged consumable product", "SDKTemplate.Scenario3_UnmanagedConsumable" },
    { "Managed consumable product", "SDKTemplate.Scenario4_ConsumableProduct" },
    { "User collection", "SDKTemplate.Scenario5_UserCollection" },
    { "App listing URI", "SDKTemplate.Scenario6_AppListingURI" },
    { "Business to Business", "SDKTemplate.Scenario7_B2B" }
}; 

int64_t Utils::DaysUntil(DateTime endDate)
{
    auto calendar = ref new Windows::Globalization::Calendar();
    calendar->SetToNow();
    return static_cast<int>((endDate.UniversalTime - calendar->GetDateTime().UniversalTime) / 864000000000);
}

Vector<ItemDetails^>^ Utils::CreateProductListFromQueryResult(StoreProductQueryResult^ addOns, String^ description)
{
    auto productList = ref new Vector<ItemDetails^>;

    if (addOns->ExtendedError.Value != S_OK)
    {
        ReportExtendedError(addOns->ExtendedError);
    }
    else if (addOns->Products->Size == 0)
    {
        MainPage::Current->NotifyUser("No configured " + description + " found for this Store Product.", NotifyType::ErrorMessage);
    }
    else
    {
        for (IKeyValuePair<String^, StoreProduct^>^ addOn : addOns->Products)
        {
            StoreProduct^ product = addOn->Value;
            productList->Append(ref new ItemDetails(product));
        }
    }

    return productList;
}

void Utils::ReportExtendedError(HResult extendedError)
{
    static const HRESULT IAP_E_UNEXPECTED = 0x803f6107L;

    String^ message;
    if (extendedError.Value == IAP_E_UNEXPECTED)
    {
        message = "This sample has not been properly configured. See the README for instructions.";
    }
    else
    {
        // The user may be offline or there might be some other server failure.
        message = "ExtendedError: " + extendedError.Value.ToString();
    }
    MainPage::Current->NotifyUser(message, NotifyType::ErrorMessage);
}

bool BindingUtils::IsNonNull(Platform::Object^ o)
{
    return o != nullptr;
}

