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
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// A page for fifth scenario.
    /// </summary>
    public sealed partial class Scenario5_AdvancedConsumableProduct : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser().
        MainPage rootPage = MainPage.Current;

        class ProductPurchaseInfo
        {
            public Guid tempTransactionId = Guid.Empty;
            public int numPurchases = 0;
            public int numFulfillments = 0;

            public string productId;
            public Run nameRun;
            public Run priceRun;
            public Run purchasesRun;
            public Run fulfilledRun;

            public List<Guid> consumedTransactionIds = new List<Guid>();
        };

        private ProductPurchaseInfo[] purchaseInfos;

        public Scenario5_AdvancedConsumableProduct()
        {
            this.InitializeComponent();

            // Initialize our table of consumables.
            purchaseInfos = new ProductPurchaseInfo[] {
                new ProductPurchaseInfo {
                    productId = "product1",
                    nameRun = Product1Name,
                    priceRun = Product1Price,
                    purchasesRun = Product1Purchases,
                    fulfilledRun = Product1Fulfilled,
                },
                new ProductPurchaseInfo
                {
                    productId = "product2",
                    nameRun = Product2Name,
                    priceRun = Product2Price,
                    purchasesRun = Product2Purchases,
                    fulfilledRun = Product2Fulfilled,
                },
            };
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            await MainPage.ConfigureSimulatorAsync("in-app-purchase-consumables-advanced.xml");

            try
            {
                ListingInformation listing = await CurrentAppSimulator.LoadListingInformationAsync();

                // Initialize the UI for our consumables.
                foreach (ProductPurchaseInfo info in purchaseInfos)
                {
                    var product = listing.ProductListings[info.productId];
                    info.nameRun.Text = product.Name;
                    info.priceRun.Text = product.FormattedPrice;
                }

                rootPage.NotifyUser("", NotifyType.StatusMessage);
            }
            catch (Exception)
            {
                rootPage.NotifyUser("LoadListingInformationAsync API call failed", NotifyType.ErrorMessage);
            }

            // recover any unfulfilled consumables
            try
            {
                IReadOnlyList<UnfulfilledConsumable> consumables = await CurrentAppSimulator.GetUnfulfilledConsumablesAsync();
                foreach (UnfulfilledConsumable consumable in consumables)
                {
                    foreach (ProductPurchaseInfo info in purchaseInfos)
                    {
                        if (info.productId == consumable.ProductId)
                        {
                            // This is a consumable which the user purchased but which has not yet been fulfilled.
                            // Update our statistics so we know that there is a purchase pending.
                            info.numPurchases++;

                            // This is where you would normally grant the user their consumable content and call currentApp.reportConsumableFulfillment.
                            // For demonstration purposes, we leave the purchase unfulfilled.
                            info.tempTransactionId = consumable.TransactionId;
                        }
                    }
                }
            }
            catch (Exception)
            {
                rootPage.NotifyUser("GetUnfulfilledConsumablesAsync API call failed", NotifyType.ErrorMessage);
            }
            UpdateStatistics();
        }

        private async void ShowUnfulfilledConsumables()
        {
            try
            {
                IReadOnlyList<UnfulfilledConsumable> consumables = await CurrentAppSimulator.GetUnfulfilledConsumablesAsync();
                string logMessage = "Number of unfulfilled consumables: " + consumables.Count;

                foreach (UnfulfilledConsumable consumable in consumables)
                {
                    logMessage += "\nProduct Id: " + consumable.ProductId + " Transaction Id: " + consumable.TransactionId;
                    // This is where you would grant the user their consumable content and call currentApp.reportConsumableFulfillment
                    // For demonstration purposes, we leave the purchase unfulfilled.
                }
                rootPage.NotifyUser(logMessage, NotifyType.StatusMessage);
            }
            catch (Exception)
            {
                rootPage.NotifyUser("GetUnfulfilledConsumablesAsync API call failed", NotifyType.ErrorMessage);
            }
        }

        private async void BuyProduct(ProductPurchaseInfo info)
        {
            rootPage.NotifyUser("Buying " + info.nameRun.Text + "...", NotifyType.StatusMessage);
            try
            {
                PurchaseResults purchaseResults = await CurrentAppSimulator.RequestProductPurchaseAsync(info.productId);
                switch (purchaseResults.Status)
                {
                    case ProductPurchaseStatus.Succeeded:
                        info.numPurchases++;
                        info.tempTransactionId = purchaseResults.TransactionId;
                        rootPage.NotifyUser("You bought " + info.nameRun.Text + ". Transaction Id: " + purchaseResults.TransactionId, NotifyType.StatusMessage);

                        // Normally you would grant the user their content here, and then call currentApp.reportConsumableFulfillment
                        // For demonstration purposes, we leave the purchase unfulfilled.
                        break;
                    case ProductPurchaseStatus.NotFulfilled:
                        info.tempTransactionId = purchaseResults.TransactionId;
                        rootPage.NotifyUser("You have an unfulfilled copy of " + info.nameRun.Text + ". Fulfill it before purchasing another one.", NotifyType.StatusMessage);

                        // Normally you would grant the user their content here, and then call currentApp.reportConsumableFulfillment
                        // For demonstration purposes, we leave the purchase unfulfilled.
                        break;
                    case ProductPurchaseStatus.NotPurchased:
                        rootPage.NotifyUser(info.nameRun.Text + " was not purchased.", NotifyType.StatusMessage);
                        break;
                }
            }
            catch (Exception)
            {
                rootPage.NotifyUser("Unable to buy " + info.nameRun.Text + ".", NotifyType.ErrorMessage);
            }
            UpdateStatistics();
        }

        private async void FulfillProduct(ProductPurchaseInfo info)
        {
            if (!IsLocallyFulfilled(info, info.tempTransactionId))
            {
                GrantFeatureLocally(info, info.tempTransactionId);
            }

            try
            {
                FulfillmentResult result = await CurrentAppSimulator.ReportConsumableFulfillmentAsync(info.productId, info.tempTransactionId);

                switch (result)
                {
                    case FulfillmentResult.Succeeded:
                        info.numFulfillments++;
                        rootPage.NotifyUser(info.nameRun.Text + " was fulfilled. You are now able to buy another one.", NotifyType.StatusMessage);
                        break;
                    case FulfillmentResult.NothingToFulfill:
                        rootPage.NotifyUser("There is nothing to fulfill. You must purchase " + info.nameRun.Text + " before it can be fulfilled.", NotifyType.StatusMessage);
                        break;
                    case FulfillmentResult.PurchasePending:
                        rootPage.NotifyUser("Purchase hasn't completed yet. Wait and try again.", NotifyType.StatusMessage);
                        break;
                    case FulfillmentResult.PurchaseReverted:
                        rootPage.NotifyUser("Purchase was reverted before fulfillment.", NotifyType.StatusMessage);
                        // Since the user's purchase was revoked, they got their money back.
                        // You may want to revoke the user's access to the consumable content that was granted.
                        break;
                    case FulfillmentResult.ServerError:
                        rootPage.NotifyUser("There was an error when fulfilling.", NotifyType.StatusMessage);
                        break;
                }
            }
            catch (Exception)
            {
                rootPage.NotifyUser("There was an error when fulfilling.", NotifyType.ErrorMessage);
            }
            UpdateStatistics();
        }

        void BuyProduct1()
        {
            BuyProduct(purchaseInfos[0]);
        }

        void FulfillProduct1()
        {
            FulfillProduct(purchaseInfos[0]);
        }

        void BuyProduct2()
        {
            BuyProduct(purchaseInfos[1]);
        }

        void FulfillProduct2()
        {
            FulfillProduct(purchaseInfos[1]);
        }

        void UpdateStatistics()
        {
            foreach (ProductPurchaseInfo info in purchaseInfos)
            {
                info.purchasesRun.Text = info.numPurchases.ToString();
                info.fulfilledRun.Text = info.numFulfillments.ToString();
            }
        }
        private void GrantFeatureLocally(ProductPurchaseInfo info, Guid transactionId)
        {
            info.consumedTransactionIds.Add(transactionId);

            // Grant the user their content. You will likely increase some kind of gold/coins/some other asset count.
        }

        private Boolean IsLocallyFulfilled(ProductPurchaseInfo info, Guid transactionId)
        {
            return info.consumedTransactionIds.Contains(transactionId);
        }
    }
}
