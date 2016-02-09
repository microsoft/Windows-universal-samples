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
using namespace Windows::ApplicationModel::Store;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Navigation;

Scenario4_ConsumableProduct::Scenario4_ConsumableProduct()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario4_ConsumableProduct::OnNavigatedTo(NavigationEventArgs^ e)
{
    ConfigureSimulatorAsync("in-app-purchase-consumables.xml").then([]()
    {
        return create_task(CurrentAppSimulator::LoadListingInformationAsync());
    }).then([this](task<ListingInformation^> currentTask)
    {
        try
        {
            ListingInformation^ listing = currentTask.get();
            auto product1 = listing->ProductListings->Lookup("product1");
            Product1Name->Text = product1->Name;
            Product1Price->Text = product1->FormattedPrice;
            rootPage->NotifyUser("", NotifyType::StatusMessage);
        }
        catch (Platform::Exception^ exception)
        {
            rootPage->NotifyUser("LoadListingInformationAsync API call failed", NotifyType::ErrorMessage);
        }
    });
}

void Scenario4_ConsumableProduct::BuyAndFulfillProduct1()
{
    rootPage->NotifyUser("Buying Product 1...", NotifyType::StatusMessage);
    create_task(CurrentAppSimulator::RequestProductPurchaseAsync("product1")).then([this](task<PurchaseResults^> currentTask)
    {
        try
        {
            PurchaseResults^ results = currentTask.get();
            switch (results->Status)
            {
            case ProductPurchaseStatus::Succeeded:
                GrantFeatureLocally(results->TransactionId);
                FulfillProduct1("product1", results->TransactionId);
                break;
            case ProductPurchaseStatus::NotFulfilled:
                // The purchase failed because we haven't confirmed fulfillment of a previous purchase.
                // Fulfill it now.
                if (!IsLocallyFulfilled(results->TransactionId))
                {
                    GrantFeatureLocally(results->TransactionId);
                }
                FulfillProduct1("product1", results->TransactionId);
                break;
            case ProductPurchaseStatus::NotPurchased:
                rootPage->NotifyUser("Product 1 was not purchased.", NotifyType::StatusMessage);
                break;
            }
        }
        catch (Platform::Exception^ exception)
        {
            rootPage->NotifyUser("Unable to buy Product 1.", NotifyType::ErrorMessage);
        }
    });
}

void Scenario4_ConsumableProduct::FulfillProduct1(Platform::String^ productId, Platform::Guid transactionId)
{
    create_task(CurrentAppSimulator::ReportConsumableFulfillmentAsync(productId, transactionId)).then([this](task<FulfillmentResult> currentTask)
    {
        try
        {
            FulfillmentResult result = currentTask.get();
            switch (result)
            {
            case FulfillmentResult::Succeeded:
                rootPage->NotifyUser("You bought and fulfilled product 1.", NotifyType::StatusMessage);
                break;
            case FulfillmentResult::NothingToFulfill:
                rootPage->NotifyUser("There is no purchased product 1 to fulfill.", NotifyType::StatusMessage);
                break;
            case FulfillmentResult::PurchasePending:
                rootPage->NotifyUser("You bought product 1. The purchase is pending so we cannot fulfill the product.", NotifyType::StatusMessage);
                break;
            case FulfillmentResult::PurchaseReverted:
                rootPage->NotifyUser("You bought product 1. But your purchase has been reverted.", NotifyType::StatusMessage);
                // Since the user's purchase was revoked, they got their money back.
                // You may want to revoke the user's access to the consumable content that was granted.
                break;
            case FulfillmentResult::ServerError:
                rootPage->NotifyUser("You bought product 1. There was an error when fulfilling.", NotifyType::StatusMessage);
                break;
            }
        }
        catch (Platform::Exception^ exception)
        {
            rootPage->NotifyUser("You bought Product 1. There was an error when fulfilling.", NotifyType::ErrorMessage);
        }
    });
}

void Scenario4_ConsumableProduct::GrantFeatureLocally(Platform::Guid transactionId)
{
    consumedTransactionIds->Append(transactionId);

    // Grant the user their content. You will likely increase some kind of gold/coins/some other asset count.
    numberOfConsumablesPurchased++;
    PurchaseCount->Text = numberOfConsumablesPurchased.ToString();
}

bool Scenario4_ConsumableProduct::IsLocallyFulfilled(Platform::Guid transactionId)
{
    unsigned int index;
    return consumedTransactionIds->IndexOf(transactionId, &index);
}
