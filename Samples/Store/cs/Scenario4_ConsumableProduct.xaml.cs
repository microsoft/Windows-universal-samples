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
    /// A page for fourth scenario.
    /// </summary>
    public sealed partial class Scenario4_ConsumableProduct : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;
        private int numberOfConsumablesPurchased = 0;
        private HashSet<Guid> consumedTransactionIds = new HashSet<Guid>();

        public Scenario4_ConsumableProduct()
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
            await MainPage.ConfigureSimulatorAsync("in-app-purchase-consumables.xml");

            try
            {
                ListingInformation listing = await CurrentAppSimulator.LoadListingInformationAsync();
                var product1 = listing.ProductListings["product1"];
                Product1Name.Text = product1.Name;
                Product1Price.Text = product1.FormattedPrice;
                rootPage.NotifyUser("", NotifyType.StatusMessage);
            }
            catch (Exception)
            {
                rootPage.NotifyUser("LoadListingInformationAsync API call failed", NotifyType.ErrorMessage);
            }
        }

        private async void BuyAndFulfillProduct1()
        {
            rootPage.NotifyUser("Buying Product 1...", NotifyType.StatusMessage);
            try
            {
                PurchaseResults purchaseResults = await CurrentAppSimulator.RequestProductPurchaseAsync("product1");
                switch (purchaseResults.Status)
                {
                    case ProductPurchaseStatus.Succeeded:
                        GrantFeatureLocally(purchaseResults.TransactionId);
                        FulfillProduct1(purchaseResults.TransactionId);
                        break;
                    case ProductPurchaseStatus.NotFulfilled:
                        // The purchase failed because we haven't confirmed fulfillment of a previous purchase.
                        // Fulfill it now.
                        if (!IsLocallyFulfilled(purchaseResults.TransactionId))
                        {
                            GrantFeatureLocally(purchaseResults.TransactionId);
                        }
                        FulfillProduct1(purchaseResults.TransactionId);
                        break;
                    case ProductPurchaseStatus.NotPurchased:
                        rootPage.NotifyUser("Product 1 was not purchased.", NotifyType.StatusMessage);
                        break;
                }
            }
            catch (Exception)
            {
                rootPage.NotifyUser("Unable to buy Product 1.", NotifyType.ErrorMessage);
            }
        }

        private async void FulfillProduct1(Guid transactionId)
        {
            try
            {
                FulfillmentResult result = await CurrentAppSimulator.ReportConsumableFulfillmentAsync("product1", transactionId);
                switch (result)
                {
                    case FulfillmentResult.Succeeded:
                        rootPage.NotifyUser("You bought and fulfilled product 1.", NotifyType.StatusMessage);
                        break;
                    case FulfillmentResult.NothingToFulfill:
                        rootPage.NotifyUser("There is no purchased product 1 to fulfill.", NotifyType.StatusMessage);
                        break;
                    case FulfillmentResult.PurchasePending:
                        rootPage.NotifyUser("You bought product 1. The purchase is pending so we cannot fulfill the product.", NotifyType.StatusMessage);
                        break;
                    case FulfillmentResult.PurchaseReverted:
                        rootPage.NotifyUser("You bought product 1, but your purchase has been reverted.", NotifyType.StatusMessage);
                        // Since the user's purchase was revoked, they got their money back.
                        // You may want to revoke the user's access to the consumable content that was granted.
                        break;
                    case FulfillmentResult.ServerError:
                        rootPage.NotifyUser("You bought product 1. There was an error when fulfilling.", NotifyType.StatusMessage);
                        break;
                }
            }
            catch (Exception)
            {
                rootPage.NotifyUser("You bought Product 1. There was an error when fulfilling.", NotifyType.ErrorMessage);
            }
        }

        private void GrantFeatureLocally(Guid transactionId)
        {
            consumedTransactionIds.Add(transactionId);

            // Grant the user their content. You will likely increase some kind of gold/coins/some other asset count.
            numberOfConsumablesPurchased++;
            PurchaseCount.Text = numberOfConsumablesPurchased.ToString();
        }

        private bool IsLocallyFulfilled(Guid transactionId)
        {
            return consumedTransactionIds.Contains(transactionId);
        }
    }
}
