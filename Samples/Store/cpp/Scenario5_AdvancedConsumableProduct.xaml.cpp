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
#include "Scenario5_AdvancedConsumableProduct.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Store;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml::Navigation;

Scenario5_AdvancedConsumableProduct::Scenario5_AdvancedConsumableProduct()
{
    InitializeComponent();

    // Initialize our table of consumables.
    purchaseInfos[0].productId = "product1";
    purchaseInfos[0].nameRun = Product1Name;
    purchaseInfos[0].priceRun = Product1Price;
    purchaseInfos[0].purchasesRun = Product1Purchases;
    purchaseInfos[0].fulfilledRun = Product1Fulfilled;

    purchaseInfos[1].productId = "product2";
    purchaseInfos[1].nameRun = Product2Name;
    purchaseInfos[1].priceRun = Product2Price;
    purchaseInfos[1].purchasesRun = Product2Purchases;
    purchaseInfos[1].fulfilledRun = Product2Fulfilled;
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario5_AdvancedConsumableProduct::OnNavigatedTo(NavigationEventArgs^ e)
{
    ConfigureSimulatorAsync("in-app-purchase-consumables-advanced.xml").then([]()
    {
        return create_task(CurrentAppSimulator::LoadListingInformationAsync());
    }).then([this](task<ListingInformation^> currentTask)
    {
        try
        {
            ListingInformation^ listing = currentTask.get();

            // Initialize the UI for our consumables.
            for (ProductPurchaseInfo& info : purchaseInfos)
            {
                auto product = listing->ProductListings->Lookup(info.productId);
                info.nameRun->Text = product->Name;
                info.priceRun->Text = product->FormattedPrice;
            }

            rootPage->NotifyUser("", NotifyType::StatusMessage);
        }
        catch (Platform::Exception^ exception)
        {
            rootPage->NotifyUser("LoadListingInformationAsync API call failed", NotifyType::ErrorMessage);
        }

        // recover already purchased consumables
        return create_task(CurrentAppSimulator::GetUnfulfilledConsumablesAsync());
    }).then([this](task<IVectorView<UnfulfilledConsumable^>^> currentTask)
    {
        try
        {
            IVectorView<UnfulfilledConsumable^>^ consumables = currentTask.get();
            for (UnfulfilledConsumable^ consumable : consumables)
            {
                for (ProductPurchaseInfo& info : purchaseInfos)
                {
                    if (info.productId == consumable->ProductId)
                    {
                        // This is a consumable which the user purchased but which has not yet been fulfilled.
                        // Update our statistics so we know that there is a purchase pending.
                        info.numPurchases++;

                        // This is where you would normally grant the user their consumable content and call currentApp.reportConsumableFulfillment.
                        // For demonstration purposes, we leave the purchase unfulfilled.
                        info.tempTransactionId = consumable->TransactionId;
                    }
                }
            }
        }
        catch (Platform::Exception^ exception)
        {
            rootPage->NotifyUser("GetUnfulfilledConsumables API call failed", NotifyType::ErrorMessage);
        }
        UpdateStatistics();
    });
}

void Scenario5_AdvancedConsumableProduct::ShowUnfulfilledConsumables()
{
    create_task(CurrentAppSimulator::GetUnfulfilledConsumablesAsync()).then([this](task<IVectorView<UnfulfilledConsumable^>^> currentTask)
    {
        try
        {
            Windows::Foundation::Collections::IVectorView<UnfulfilledConsumable^>^ consumables = currentTask.get();
            Platform::String^ logMessage = "Number of unfulfilled consumables: " + consumables->Size;

            for (UnfulfilledConsumable^ consumable : consumables)
            {
                logMessage += "\nProduct Id: " + consumable->ProductId + " Transaction Id: " + consumable->TransactionId;
                // This is where you would grant the user their consumable content and call currentApp.reportConsumableFulfillment.
                // For demonstration purposes, we leave the purchase unfulfilled.
            }
            rootPage->NotifyUser(logMessage, NotifyType::StatusMessage);
        }
        catch (Platform::Exception^ exception)
        {
            rootPage->NotifyUser("GetUnfulfilledConsumables API call failed", NotifyType::ErrorMessage);
        }
    });
}

void Scenario5_AdvancedConsumableProduct::BuyProduct(ProductPurchaseInfo& info)
{
    rootPage->NotifyUser("Buying " + info.nameRun->Text + "...", NotifyType::StatusMessage);
    create_task(CurrentAppSimulator::RequestProductPurchaseAsync(info.productId)).then([this, &info](task<PurchaseResults^> currentTask)
    {
        try
        {
            PurchaseResults^ results = currentTask.get();
            switch (results->Status)
            {
            case ProductPurchaseStatus::Succeeded:
                info.numPurchases++;
                info.tempTransactionId = results->TransactionId;
                rootPage->NotifyUser("You bought " + info.nameRun->Text + ". Transaction Id: " + results->TransactionId, NotifyType::StatusMessage);

                // Normally you would grant the user their content here, and then call currentApp.reportConsumableFulfillment.
                // For demonstration purposes, we leave the purchase unfulfilled.
                break;
            case ProductPurchaseStatus::NotFulfilled:
                info.tempTransactionId = results->TransactionId;
                rootPage->NotifyUser("You have an unfulfilled purchase of " + info.nameRun->Text + ". Fulfill it before purchasing another one.", NotifyType::StatusMessage);

                // Normally you would grant the user their content here, and then call currentApp.reportConsumableFulfillment.
                // For demonstration purposes, we leave the purchase unfulfilled.
                break;
            case ProductPurchaseStatus::NotPurchased:
                rootPage->NotifyUser(info.nameRun->Text + " was not purchased.", NotifyType::StatusMessage);
                break;
            }
        }
        catch (Platform::Exception^ exception)
        {
            rootPage->NotifyUser("Unable to buy " + info.nameRun->Text + ".", NotifyType::ErrorMessage);
        }
        UpdateStatistics();
    });
}

void Scenario5_AdvancedConsumableProduct::FulfillProduct(ProductPurchaseInfo& info)
{
    if (!IsLocallyFulfilled(info, info.tempTransactionId))
    {
        GrantFeatureLocally(info, info.tempTransactionId);
    }
    create_task(CurrentAppSimulator::ReportConsumableFulfillmentAsync(info.productId, info.tempTransactionId)).then([this, &info](task<FulfillmentResult> currentTask)
    {
        try
        {
            FulfillmentResult result = currentTask.get();
            switch (result)
            {
            case FulfillmentResult::Succeeded:
                info.numFulfillments++;
                rootPage->NotifyUser(info.nameRun->Text + " was fulfilled. You are now able to buy another one.", NotifyType::StatusMessage);
                break;
            case FulfillmentResult::NothingToFulfill:
                rootPage->NotifyUser("There is nothing to fulfill. You must purchase " + info.nameRun->Text + " before it can be fulfilled.", NotifyType::StatusMessage);
                break;
            case FulfillmentResult::PurchasePending:
                rootPage->NotifyUser("Purchase hasn't completed yet. Wait and try again.", NotifyType::StatusMessage);
                break;
            case FulfillmentResult::PurchaseReverted:
                rootPage->NotifyUser("Purchase was reverted before fulfillment.", NotifyType::StatusMessage);
                // Since the user's purchase was revoked, they got their money back.
                // You may want to revoke the user's access to the consumable content that was granted.
                break;
            case FulfillmentResult::ServerError:
                rootPage->NotifyUser("There was an error when fulfilling.", NotifyType::StatusMessage);
                break;
            }
        }
        catch (Platform::Exception^ exception)
        {
            rootPage->NotifyUser("There was an error when fulfilling.", NotifyType::ErrorMessage);
        }
        UpdateStatistics();
    });
}

void Scenario5_AdvancedConsumableProduct::BuyProduct1()
{
    BuyProduct(purchaseInfos[0]);
}

void Scenario5_AdvancedConsumableProduct::FulfillProduct1()
{
    FulfillProduct(purchaseInfos[0]);
}

void Scenario5_AdvancedConsumableProduct::BuyProduct2()
{
    BuyProduct(purchaseInfos[1]);
}

void Scenario5_AdvancedConsumableProduct::FulfillProduct2()
{
    FulfillProduct(purchaseInfos[1]);
}

void Scenario5_AdvancedConsumableProduct::UpdateStatistics()
{
    for (ProductPurchaseInfo& info : purchaseInfos)
    {
        info.purchasesRun->Text = info.numPurchases.ToString();
        info.fulfilledRun->Text = info.numFulfillments.ToString();
    }
}

void Scenario5_AdvancedConsumableProduct::GrantFeatureLocally(ProductPurchaseInfo& info, Platform::Guid transactionId)
{
    info.consumedTransactionIds->Append(transactionId);

    // Grant the user their content. You will likely increase some kind of gold/coins/some other asset count.
}

bool Scenario5_AdvancedConsumableProduct::IsLocallyFulfilled(ProductPurchaseInfo& info, Platform::Guid transactionId)
{
    unsigned int index;
    return info.consumedTransactionIds->IndexOf(transactionId, &index);
}
