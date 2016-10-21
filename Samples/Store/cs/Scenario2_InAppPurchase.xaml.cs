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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario2_InAppPurchase : Page
    {
        private MainPage rootPage = MainPage.Current;
        private StoreContext storeContext = StoreContext.GetDefault();

        public Scenario2_InAppPurchase()
        {
            this.InitializeComponent();
        }

        private async void GetAssociatedProductsButton_Click(object sender, RoutedEventArgs e)
        {
            // Create a filtered list of the product AddOns I care about
            string[] filterList = new string[] { "Consumable", "Durable", "UnmanagedConsumable" };

            // Get list of Add Ons this app can sell, filtering for the types we know about
            StoreProductQueryResult addOns = await storeContext.GetAssociatedStoreProductsAsync(filterList);

            ProductsListView.ItemsSource = Utils.CreateProductListFromQueryResult(addOns, "Add-Ons");
        }

        private async void PurchaseAddOnButton_Click(object sender, RoutedEventArgs e)
        {
            var item = (ItemDetails)ProductsListView.SelectedItem;

            StorePurchaseResult result = await storeContext.RequestPurchaseAsync(item.StoreId);
            if (result.ExtendedError != null)
            {
                Utils.ReportExtendedError(result.ExtendedError);
                return;
            }

            switch (result.Status)
            {
                case StorePurchaseStatus.AlreadyPurchased:
                    rootPage.NotifyUser($"You already bought this AddOn.", NotifyType.ErrorMessage);
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
    }
}
