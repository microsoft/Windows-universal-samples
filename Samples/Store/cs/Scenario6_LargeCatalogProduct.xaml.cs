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

using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.ApplicationModel.Store;
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// A page for sixth scenario.
    /// </summary>
    public sealed partial class Scenario6_LargeCatalogProduct : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser().
        MainPage rootPage = MainPage.Current;

        private HashSet<Guid> consumedTransactionIds = new HashSet<Guid>();
        private string product1ListingName = "";

        public Scenario6_LargeCatalogProduct()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            await MainPage.ConfigureSimulatorAsync("in-app-purchase-large-catalog.xml");

            try
            {
                ListingInformation listing = await CurrentAppSimulator.LoadListingInformationAsync();
                var product1 = listing.ProductListings["product1"];
                ProductPrice.Text = product1.FormattedPrice;
                product1ListingName = product1.Name;
                rootPage.NotifyUser("", NotifyType.StatusMessage);
            }
            catch (Exception)
            {
                rootPage.NotifyUser("LoadListingInformationAsync API call failed", NotifyType.ErrorMessage);
            }
            ProductNameChanged();
        }

        private string BuildProductNameForDisplay()
        {
            string displayName = DisplayNameTextBox.Text;
            return string.IsNullOrEmpty(displayName) ? product1ListingName : displayName;
        }

        private void ProductNameChanged()
        {
            ProductName.Text = BuildProductNameForDisplay();
        }

        private string BuildOfferStringForDisplay(string offerId)
        {
            if (string.IsNullOrEmpty(offerId))
            {
                return " with no offer id";
            }
            else
            {
                return " with offer id " + offerId;
            }
        }

        private async void BuyAndFulfillProduct()
        {
            string offerId = OfferIdTextBox.Text;
            string displayPropertiesName = DisplayNameTextBox.Text;
            var displayProperties = new ProductPurchaseDisplayProperties(displayPropertiesName);

            rootPage.NotifyUser("Buying Product 1" + BuildOfferStringForDisplay(offerId) + "...", NotifyType.StatusMessage);
            try
            {
                PurchaseResults purchaseResults = await CurrentAppSimulator.RequestProductPurchaseAsync("product1", offerId, displayProperties);
                switch (purchaseResults.Status)
                {
                    case ProductPurchaseStatus.Succeeded:
                        GrantFeatureLocally(purchaseResults.TransactionId);
                        FulfillProduct("product1", purchaseResults);
                        break;
                    case ProductPurchaseStatus.NotFulfilled:
                        if (!IsLocallyFulfilled(purchaseResults.TransactionId))
                        {
                            GrantFeatureLocally(purchaseResults.TransactionId);
                        }
                        FulfillProduct("product1", purchaseResults);
                        break;
                    case ProductPurchaseStatus.NotPurchased:
                        rootPage.NotifyUser("Product 1" + BuildOfferStringForDisplay(offerId) + " was not purchased.", NotifyType.StatusMessage);
                        break;
                }
            }
            catch (Exception)
            {
                rootPage.NotifyUser("Unable to buy Product 1" + BuildOfferStringForDisplay(offerId) + ".", NotifyType.ErrorMessage);
            }
        }

        private async void FulfillProduct(string productId, PurchaseResults purchaseResults)
        {
            string itemDescription = product1ListingName + BuildOfferStringForDisplay(purchaseResults.OfferId);
            string purchaseStringSimple = "You bought " + itemDescription + ".";
            if (purchaseResults.Status == ProductPurchaseStatus.NotFulfilled)
            {
                purchaseStringSimple = "You already purchased " + itemDescription + ".";
            }

            try
            {
                FulfillmentResult result = await CurrentAppSimulator.ReportConsumableFulfillmentAsync(productId, purchaseResults.TransactionId);
                switch (result)
                {
                    case FulfillmentResult.Succeeded:
                        if (purchaseResults.Status == ProductPurchaseStatus.NotFulfilled)
                        {
                            rootPage.NotifyUser("You already purchased " + itemDescription + " and it was just fulfilled.", NotifyType.StatusMessage);
                        }
                        else
                        {
                            rootPage.NotifyUser("You bought and fulfilled " + itemDescription, NotifyType.StatusMessage);
                        }
                        break;
                    case FulfillmentResult.NothingToFulfill:
                        rootPage.NotifyUser("There is no purchased product 1 to fulfill with that transaction id.", NotifyType.StatusMessage);
                        break;
                    case FulfillmentResult.PurchasePending:
                        rootPage.NotifyUser(purchaseStringSimple + " The purchase is pending so we cannot fulfill the product.", NotifyType.StatusMessage);
                        break;
                    case FulfillmentResult.PurchaseReverted:
                        rootPage.NotifyUser(purchaseStringSimple + " But your purchase has been reverted.", NotifyType.StatusMessage);
                        // Since the user's purchase was revoked, they got their money back.
                        // You may want to revoke the user's access to the consumable content that was granted.
                        break;
                    case FulfillmentResult.ServerError:
                        rootPage.NotifyUser(purchaseStringSimple + " There was an error when fulfilling.", NotifyType.StatusMessage);
                        break;
                }
            }
            catch (Exception)
            {
                rootPage.NotifyUser(purchaseStringSimple + " There was an error when fulfilling.", NotifyType.ErrorMessage);
            }
        }

        private void GrantFeatureLocally(Guid transactionId)
        {
            consumedTransactionIds.Add(transactionId);

            // Grant the user their content. You will likely increase some kind of gold/coins/some other asset count.
        }

        private bool IsLocallyFulfilled(Guid transactionId)
        {
            return consumedTransactionIds.Contains(transactionId);
        }
    }
}
