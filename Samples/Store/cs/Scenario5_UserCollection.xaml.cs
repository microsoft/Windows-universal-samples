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
    public sealed partial class Scenario5_UserCollection : Page
    {
        MainPage rootPage = MainPage.Current;
        private StoreContext storeContext = StoreContext.GetDefault();

        public Scenario5_UserCollection()
        {
            this.InitializeComponent();
        }

        private async void GetUserCollectionButton_Click(object sender, RoutedEventArgs e)
        {
            // Create a filtered list of the product AddOns I care about
            string[] filterList = new string[] { "Consumable", "Durable", "UnmanagedConsumable" };

            // Get user collection for this product, filtering for the types we know about
            StoreProductQueryResult collection = await storeContext.GetUserCollectionAsync(filterList);

            ProductsListView.ItemsSource = Utils.CreateProductListFromQueryResult(collection, "collection items");
        }
    }
}
