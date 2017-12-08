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
using Windows.Services.Store;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario3_UnmanagedConsumable : Page
    {
        MainPage rootPage = MainPage.Current;
        private StoreContext storeContext = StoreContext.GetDefault();

        public Scenario3_UnmanagedConsumable()
        {
            this.InitializeComponent();
        }

        private async void GetUnmanagedConsumablesButton_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            string[] filterList = new string[] { "UnmanagedConsumable" };

            // Get list of Add Ons this app can sell, filtering for the types we know about.
            StoreProductQueryResult addOns = await storeContext.GetAssociatedStoreProductsAsync(filterList);

            ProductsListView.ItemsSource = Utils.CreateProductListFromQueryResult(addOns, "UnmanagedConsumable Add-Ons");
        }

        private async void PurchaseAddOnButton_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            var item = (ItemDetails)ProductsListView.SelectedItem;

            var result = await storeContext.RequestPurchaseAsync(item.StoreId);
            if (result.ExtendedError != null)
            {
                Utils.ReportExtendedError(result.ExtendedError);
                return;
            }

            switch (result.Status)
            {
                case StorePurchaseStatus.AlreadyPurchased:
                    rootPage.NotifyUser($"You already bought this consumable and must fulfill it.", NotifyType.ErrorMessage);
                    break;

                case StorePurchaseStatus.Succeeded:
                    rootPage.NotifyUser($"You bought {item.Title}.", NotifyType.StatusMessage);
                    break;

                case StorePurchaseStatus.NotPurchased:
                    rootPage.NotifyUser("Product was not purchased, it may have been canceled.", NotifyType.ErrorMessage);
                    break;

                case StorePurchaseStatus.NetworkError:
                    rootPage.NotifyUser("Product was not purchased due to a network error.", NotifyType.ErrorMessage);
                    break;

                case StorePurchaseStatus.ServerError:
                    rootPage.NotifyUser("Product was not purchased due to a server error.", NotifyType.ErrorMessage);
                    break;

                default:
                    rootPage.NotifyUser("Product was not purchased due to an unknown error.", NotifyType.ErrorMessage);
                    break;
            }
        }

        private async void GetConsumableBalanceButton_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            var item = (ItemDetails)ProductsListView.SelectedItem;

            StoreConsumableResult result = await storeContext.GetConsumableBalanceRemainingAsync(item.StoreId);
            if (result.ExtendedError != null)
            {
                Utils.ReportExtendedError(result.ExtendedError);
                return;
            }

            switch (result.Status)
            {
                case StoreConsumableStatus.InsufficentQuantity: // should never get this...
                    rootPage.NotifyUser($"Insufficient Quantity! Balance Remaining: {result.BalanceRemaining}", NotifyType.ErrorMessage);
                    break;

                case StoreConsumableStatus.Succeeded:
                    rootPage.NotifyUser($"Balance Remaining: {result.BalanceRemaining}", NotifyType.StatusMessage);
                    break;

                case StoreConsumableStatus.NetworkError:
                    rootPage.NotifyUser("Network error retrieving consumable balance.", NotifyType.ErrorMessage);
                    break;

                case StoreConsumableStatus.ServerError:
                    rootPage.NotifyUser("Server error retrieving consumable balance.", NotifyType.ErrorMessage);
                    break;

                default:
                    rootPage.NotifyUser("Unknown error retrieving consumable balance.", NotifyType.ErrorMessage);
                    break;
            }
        }

        private async void FulfillConsumableButton_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            var item = (ItemDetails)ProductsListView.SelectedItem;

            // This can be used to ensure this request is never double fulfilled. The server will
            // accept only one report for this specific GUID.
            Guid trackingId = Guid.NewGuid();

            // Always use quantity of "1" to fulfill an Unmanaged consumable
            StoreConsumableResult result = await storeContext.ReportConsumableFulfillmentAsync(item.StoreId, 1, trackingId);
            if (result.ExtendedError != null)
            {
                Utils.ReportExtendedError(result.ExtendedError);
                return;
            }

            switch (result.Status)
            {
                case StoreConsumableStatus.InsufficentQuantity:
                    rootPage.NotifyUser($"Insufficient Quantity! Balance Remaining: {result.BalanceRemaining}", NotifyType.ErrorMessage);
                    break;

                case StoreConsumableStatus.Succeeded:
                    rootPage.NotifyUser($"Successful fulfillment! Balance Remaining: {result.BalanceRemaining}", NotifyType.StatusMessage);
                    break;

                case StoreConsumableStatus.NetworkError:
                    rootPage.NotifyUser("Network error fulfilling consumable.", NotifyType.ErrorMessage);
                    break;

                case StoreConsumableStatus.ServerError:
                    rootPage.NotifyUser("Server error fulfilling consumable.", NotifyType.ErrorMessage);
                    break;

                default:
                    rootPage.NotifyUser("Unknown error fulfilling consumable.", NotifyType.ErrorMessage);
                    break;
            }
        }
    }
}