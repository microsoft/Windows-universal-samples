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
    /// A page for second scenario.
    /// </summary>
    public sealed partial class Scenario2_InAppPurchase : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario2_InAppPurchase()
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
            await MainPage.ConfigureSimulatorAsync("in-app-purchase.xml");

            try
            {
                ListingInformation listing = await CurrentAppSimulator.LoadListingInformationAsync();
                var product1 = listing.ProductListings["product1"];
                Product1Name.Text = product1.Name;
                Product1Price.Text = product1.FormattedPrice;

                var product2 = listing.ProductListings["product2"];
                Product2Name.Text = product2.Name;
                Product2Price.Text = product2.FormattedPrice;
            }
            catch (Exception)
            {
                rootPage.NotifyUser("LoadListingInformationAsync API call failed", NotifyType.ErrorMessage);
            }
        }

        private void TestProduct(string productId, string productName)
        {
            LicenseInformation licenseInformation = CurrentAppSimulator.LicenseInformation;
            var productLicense = licenseInformation.ProductLicenses[productId];
            if (productLicense.IsActive)
            {
                rootPage.NotifyUser("You can use " + productName + ".", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("You don't own " + productName + ".", NotifyType.ErrorMessage);
            }
        }

        private async Task BuyProductAsync(string productId, string productName)
        {
            LicenseInformation licenseInformation = CurrentAppSimulator.LicenseInformation;
            if (!licenseInformation.ProductLicenses[productId].IsActive)
            {
                rootPage.NotifyUser("Buying " + productName + "...", NotifyType.StatusMessage);
                try
                {
                    await CurrentAppSimulator.RequestProductPurchaseAsync(productId);
                    if (licenseInformation.ProductLicenses[productId].IsActive)
                    {
                        rootPage.NotifyUser("You bought " + productName + ".", NotifyType.StatusMessage);
                    }
                    else
                    {
                        rootPage.NotifyUser(productName + " was not purchased.", NotifyType.StatusMessage);
                    }
                }
                catch (Exception)
                {
                    rootPage.NotifyUser("Unable to buy " + productName + ".", NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("You already own " + productName + ".", NotifyType.ErrorMessage);
            }
        }

        private void TestProduct1()
        {
            TestProduct("product1", Product1Name.Text);
        }

        private async void BuyProduct1()
        {
            await BuyProductAsync("product1", Product1Name.Text);
        }

        private void TestProduct2()
        {
            TestProduct("product2", Product2Name.Text);
        }

        private async void BuyProduct2()
        {
            await BuyProductAsync("product2", Product2Name.Text);
        }
    }
}
