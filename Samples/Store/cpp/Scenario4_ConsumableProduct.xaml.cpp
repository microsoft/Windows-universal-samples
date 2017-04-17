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
#include "Scenario4_ConsumableProduct.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Services::Store;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

Scenario4_ConsumableProduct::Scenario4_ConsumableProduct()
{
    InitializeComponent();

    storeContext = StoreContext::GetDefault();
}


void Scenario4_ConsumableProduct::GetManagedConsumablesButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    auto filterList = ref new Vector<String^>({ "Consumable" });

    // Get list of Add Ons this app can sell, filtering for the types we know about.
    create_task(storeContext->GetAssociatedStoreProductsAsync(filterList)).then([this](StoreProductQueryResult^ addOns)
    {
        ProductsListView->ItemsSource = Utils::CreateProductListFromQueryResult(addOns, "Consumable Add-Ons");
    }, task_continuation_context::get_current_winrt_context());
}

void Scenario4_ConsumableProduct::PurchaseAddOnButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    auto item = safe_cast<ItemDetails^>(ProductsListView->SelectedItem);

    create_task(storeContext->RequestPurchaseAsync(item->StoreId)).then([this, item](StorePurchaseResult^ result)
    {
        if (result->ExtendedError.Value != S_OK)
        {
            Utils::ReportExtendedError(result->ExtendedError);
            return;
        }

        switch (result->Status)
        {
        case StorePurchaseStatus::AlreadyPurchased: // should never get this for a managed consumable since they are stackable
            rootPage->NotifyUser("You already bought this consumable.", NotifyType::ErrorMessage);
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

void Scenario4_ConsumableProduct::GetConsumableBalanceButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    auto item = safe_cast<ItemDetails^>(ProductsListView->SelectedItem);

    create_task(storeContext->GetConsumableBalanceRemainingAsync(item->StoreId)).then([this](StoreConsumableResult^ result)
    {
        if (result->ExtendedError.Value != S_OK)
        {
            Utils::ReportExtendedError(result->ExtendedError);
            return;
        }

        switch (result->Status)
        {
        case StoreConsumableStatus::InsufficentQuantity: // should never get this...
            rootPage->NotifyUser("Insufficient Quantity! Balance Remaining: " + result->BalanceRemaining, NotifyType::ErrorMessage);
            break;

        case StoreConsumableStatus::Succeeded:
            rootPage->NotifyUser("Balance Remaining: " + result->BalanceRemaining, NotifyType::StatusMessage);
            break;

        case StoreConsumableStatus::NetworkError:
            rootPage->NotifyUser("Network error retrieving consumable balance.", NotifyType::ErrorMessage);
            break;

        case StoreConsumableStatus::ServerError:
            rootPage->NotifyUser("Server error retrieving consumable balance.", NotifyType::ErrorMessage);
            break;

        default:
            rootPage->NotifyUser("Unknown error retrieving consumable balance.", NotifyType::ErrorMessage);
            break;
        }
    }, task_continuation_context::get_current_winrt_context());
}

void Scenario4_ConsumableProduct::FulfillConsumableButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    auto item = safe_cast<ItemDetails^>(ProductsListView->SelectedItem);

    unsigned int quantity = _wtoi(safe_cast<String^>(QuantityComboBox->SelectedValue)->Data());

    // This can be used to ensure this request is never double fulfilled. The server will
    // only accept one report for this specific GUID.
    GUID guid;
    if (FAILED(CoCreateGuid(&guid)))
    {
        rootPage->NotifyUser("Failed to create a tracking GUID.", NotifyType::ErrorMessage);
        return;
    }

    Guid trackingId(guid);

    create_task(storeContext->ReportConsumableFulfillmentAsync(item->StoreId, quantity, trackingId)).then([this](StoreConsumableResult^ result)
    {
        if (result->ExtendedError.Value != S_OK)
        {
            Utils::ReportExtendedError(result->ExtendedError);
            return;
        }

        switch (result->Status)
        {
        case StoreConsumableStatus::InsufficentQuantity:
            rootPage->NotifyUser("Insufficient Quantity! Balance Remaining: " + result->BalanceRemaining, NotifyType::ErrorMessage);
            break;

        case StoreConsumableStatus::Succeeded:
            rootPage->NotifyUser("Successful fulfillment! Balance Remaining: " + result->BalanceRemaining, NotifyType::StatusMessage);
            break;

        case StoreConsumableStatus::NetworkError:
            rootPage->NotifyUser("Network error fulfilling consumable.", NotifyType::ErrorMessage);
            break;

        case StoreConsumableStatus::ServerError:
            rootPage->NotifyUser("Server error fulfilling consumable.", NotifyType::ErrorMessage);
            break;

        default:
            rootPage->NotifyUser("Unknown error fulfilling consumable.", NotifyType::ErrorMessage);
            break;
        }
    }, task_continuation_context::get_current_winrt_context());
}
