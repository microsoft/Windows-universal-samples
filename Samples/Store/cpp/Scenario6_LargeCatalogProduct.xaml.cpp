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
#include "Scenario6_LargeCatalogProduct.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Store;
using namespace Windows::UI::Xaml::Navigation;

Scenario6_LargeCatalogProduct::Scenario6_LargeCatalogProduct()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario6_LargeCatalogProduct::OnNavigatedTo(NavigationEventArgs^ e)
{
    ConfigureSimulatorAsync("in-app-purchase-large-catalog.xml").then([]()
    {
        return create_task(CurrentAppSimulator::LoadListingInformationAsync());
    }).then([this](task<ListingInformation^> currentTask)
    {
        try
        {
            ListingInformation^ listing = currentTask.get();
            auto product1 = listing->ProductListings->Lookup("product1");
            ProductPrice->Text = product1->FormattedPrice;
            product1ListingName = product1->Name;
            rootPage->NotifyUser("", NotifyType::StatusMessage);
        }
        catch (Platform::Exception^ exception)
        {
            rootPage->NotifyUser("LoadListingInformationAsync API call failed", NotifyType::ErrorMessage);
        }
        ProductNameChanged();
    });
}

Platform::String^ Scenario6_LargeCatalogProduct::BuildProductNameForDisplay()
{
    Platform::String^ displayName = DisplayNameTextBox->Text;
    return displayName->IsEmpty() ? product1ListingName : displayName;
}

void Scenario6_LargeCatalogProduct::ProductNameChanged()
{
    ProductName->Text = BuildProductNameForDisplay();
}

Platform::String^ Scenario6_LargeCatalogProduct::BuildOfferStringForDisplay(Platform::String^ offerId)
{
    if (offerId->IsEmpty())
    {
        return " with no offer id";
    }
    else
    {
        return " with offer id " + offerId;
    }
}

void Scenario6_LargeCatalogProduct::BuyAndFulfillProduct()
{
    Platform::String^ offerId = OfferIdTextBox->Text;
    Platform::String^ displayPropertiesName = DisplayNameTextBox->Text;
    ProductPurchaseDisplayProperties^ displayProperties = ref new ProductPurchaseDisplayProperties(displayPropertiesName);

    rootPage->NotifyUser("Buying Product 1" + BuildOfferStringForDisplay(offerId) + "...", NotifyType::StatusMessage);
    create_task(CurrentAppSimulator::RequestProductPurchaseAsync("product1", offerId, displayProperties)).then([this, offerId](task<PurchaseResults^> currentTask)
    {
        try
        {
            PurchaseResults^ results = currentTask.get();
            switch (results->Status)
            {
            case ProductPurchaseStatus::Succeeded:
                GrantFeatureLocally(results->TransactionId);
                FulfillProduct("product1", results);
                break;
            case ProductPurchaseStatus::NotFulfilled:
                if (!IsLocallyFulfilled(results->TransactionId))
                {
                    GrantFeatureLocally(results->TransactionId);
                }
                FulfillProduct("product1", results);
                break;
            case ProductPurchaseStatus::NotPurchased:
                rootPage->NotifyUser("Product 1" + BuildOfferStringForDisplay(offerId) + " was not purchased.", NotifyType::StatusMessage);
                break;
            }
        }
        catch (Platform::Exception^ exception)
        {
            rootPage->NotifyUser("Unable to buy Product 1" + BuildOfferStringForDisplay(offerId) + ".", NotifyType::ErrorMessage);
        }
    });
}

void Scenario6_LargeCatalogProduct::FulfillProduct(Platform::String^ productId, PurchaseResults^ purchaseResults)
{
    create_task(CurrentAppSimulator::ReportConsumableFulfillmentAsync(productId, purchaseResults->TransactionId)).then([this, purchaseResults](task<FulfillmentResult> currentTask)
    {
        Platform::String^ itemDescription = product1ListingName + BuildOfferStringForDisplay(purchaseResults->OfferId);
        Platform::String^ purchaseStringSimple = "You bought " + itemDescription + ".";
        if (purchaseResults->Status == ProductPurchaseStatus::NotFulfilled)
        {
            purchaseStringSimple = "You already purchased " + itemDescription + ".";
        }

        try
        {
            FulfillmentResult result = currentTask.get();
            switch (result)
            {
            case FulfillmentResult::Succeeded:
                if (purchaseResults->Status == ProductPurchaseStatus::NotFulfilled)
                {
                    rootPage->NotifyUser("You had already purchased " + itemDescription + " and it was just fulfilled.", NotifyType::StatusMessage);
                }
                else
                {
                    rootPage->NotifyUser("You bought and fulfilled " + itemDescription, NotifyType::StatusMessage);
                }
                break;
            case FulfillmentResult::NothingToFulfill:
                rootPage->NotifyUser("There is no purchased product 1 to fulfill with that transaction id.", NotifyType::StatusMessage);
                break;
            case FulfillmentResult::PurchasePending:
                rootPage->NotifyUser(purchaseStringSimple + " The purchase is pending so we cannot fulfill the product.", NotifyType::StatusMessage);
                break;
            case FulfillmentResult::PurchaseReverted:
                rootPage->NotifyUser(purchaseStringSimple + " But your purchase has been reverted.", NotifyType::StatusMessage);
                // Since the user's purchase was revoked, they got their money back.
                // You may want to revoke the user's access to the consumable content that was granted.
                break;
            case FulfillmentResult::ServerError:
                rootPage->NotifyUser(purchaseStringSimple + " There was an error when fulfilling.", NotifyType::StatusMessage);
                break;
            }
        }
        catch (Platform::Exception^ exception)
        {
            rootPage->NotifyUser(purchaseStringSimple + " There was an error when fulfilling.", NotifyType::ErrorMessage);
        }
    });
}

void Scenario6_LargeCatalogProduct::GrantFeatureLocally(Platform::Guid transactionId)
{
    consumedTransactionIds->Append(transactionId);

    // Grant the user their content. You will likely increase some kind of gold/coins/some other asset count.
}

bool Scenario6_LargeCatalogProduct::IsLocallyFulfilled(Platform::Guid transactionId)
{
    unsigned int index;
    return consumedTransactionIds->IndexOf(transactionId, &index);
}
