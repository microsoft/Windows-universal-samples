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
#include "Scenario2_InAppPurchase.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Services::Store;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Navigation;

Scenario2_InAppPurchase::Scenario2_InAppPurchase()
{
    InitializeComponent();

    storeContext = StoreContext::GetDefault();
}

void Scenario2_InAppPurchase::GetAssociatedProductsButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Create a filtered list of the product AddOns I care about
    auto filterList = ref new Vector<String^>({ "Consumable" ,"Durable", "UnmanagedConsumable" });

    // Get list of Add Ons this app can sell, filtering for the types we know about
    create_task(storeContext->GetAssociatedStoreProductsAsync(filterList)).then([this](StoreProductQueryResult^ addOns)
    {
        ProductsListView->ItemsSource = Utils::CreateProductListFromQueryResult(addOns, "Add-Ons");
    }, task_continuation_context::get_current_winrt_context());
}

void Scenario2_InAppPurchase::PurchaseAddOnButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    ItemDetails^ item = safe_cast<ItemDetails^>(ProductsListView->SelectedItem);
    create_task(storeContext->RequestPurchaseAsync(item->StoreId)).then([this, item](StorePurchaseResult^ result)
    {
        if (result->ExtendedError.Value != S_OK)
        {
            Utils::ReportExtendedError(result->ExtendedError);
            return;
        }

        switch (result->Status)
        {
        case StorePurchaseStatus::AlreadyPurchased:
            rootPage->NotifyUser("You already bought this AddOn.", NotifyType::ErrorMessage);
            break;

        case StorePurchaseStatus::Succeeded:
            rootPage->NotifyUser("You bought " + item->Title + ".", NotifyType::StatusMessage);
            break;

        case StorePurchaseStatus::NotPurchased:
            rootPage->NotifyUser("Product was not purchased, it may have been canceled.", NotifyType::ErrorMessage);
            break;

        case StorePurchaseStatus::NetworkError:
            rootPage->NotifyUser("Product was not purchased due to a network error.", NotifyType::ErrorMessage);
            break;

        case StorePurchaseStatus::ServerError:
            rootPage->NotifyUser("Product was not purchased due to a server error.", NotifyType::ErrorMessage);
            break;

        default:
            rootPage->NotifyUser("Product was not purchased due to an unknown error.", NotifyType::ErrorMessage);
            break;
        }
    }, task_continuation_context::get_current_winrt_context());
}
