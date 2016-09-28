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
using Windows.Services.Store;
using Windows.UI.Core;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_TrialMode : Page
    {
        private MainPage rootPage = MainPage.Current;
        private StoreContext storeContext = null;

        public Scenario1_TrialMode()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            storeContext = StoreContext.GetDefault();
            storeContext.OfflineLicensesChanged += OfflineLicensesChanged;

            StoreProductResult result = await storeContext.GetStoreProductForCurrentAppAsync();
            if (result.ExtendedError == null)
            {
                PurchasePrice.Text = result.Product.Price.FormattedPrice;
            }

            await GetLicenseState();
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            storeContext.OfflineLicensesChanged -= OfflineLicensesChanged;
        }

        private void OfflineLicensesChanged(StoreContext sender, object args)
        {
            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, async () =>
            {
                await GetLicenseState();
            });
        }

        private async Task GetLicenseState()
        {
            StoreAppLicense license = await storeContext.GetAppLicenseAsync();

            if (license.IsActive)
            {
                if (license.IsTrial)
                {
                    LicenseMode.Text = "Trial license";
                }
                else
                {
                    LicenseMode.Text = "Full license";
                }
            }
            else
            {
                LicenseMode.Text = "Inactive license";
            }
        }

        /// <summary>
        /// Invoked when the user asks to see trial period information.
        /// </summary>
        private async void ShowTrialPeriodInformation()
        {
            StoreAppLicense license = await storeContext.GetAppLicenseAsync();
            if (license.IsActive)
            {
                if (license.IsTrial)
                {
                    int remainingTrialTime = (license.ExpirationDate - DateTime.Now).Days;
                    rootPage.NotifyUser($"You can use this app for {remainingTrialTime} more days before the trial period ends.", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("You have a full license. The trial time is not meaningful.", NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("You don't have a license. The trial time can't be determined.", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Invoked when the user asks purchase the app.
        /// </summary>
        private async void PurchaseFullLicense()
        {
            // Get app store product details
            StoreProductResult productResult = await storeContext.GetStoreProductForCurrentAppAsync();
            if (productResult.ExtendedError != null)
            {
                // The user may be offline or there might be some other server failure
                rootPage.NotifyUser($"ExtendedError: {productResult.ExtendedError.Message}", NotifyType.ErrorMessage);
                return;
            }

            rootPage.NotifyUser("Buying the full license...", NotifyType.StatusMessage);
            StoreAppLicense license = await storeContext.GetAppLicenseAsync();
            if (license.IsTrial)
            {
                StorePurchaseResult result = await productResult.Product.RequestPurchaseAsync();
                if (result.ExtendedError != null)
                {
                    rootPage.NotifyUser($"Purchase failed: ExtendedError: {result.ExtendedError.Message}", NotifyType.ErrorMessage);
                    return;
                }

                switch (result.Status)
                {
                    case StorePurchaseStatus.AlreadyPurchased:
                        rootPage.NotifyUser($"You already bought this app and have a fully-licensed version.", NotifyType.ErrorMessage);
                        break;

                    case StorePurchaseStatus.Succeeded:
                        // License will refresh automatically using the StoreContext.OfflineLicensesChanged event
                        break;

                    case StorePurchaseStatus.NotPurchased:
                        rootPage.NotifyUser("Product was not purchased, it may have been canceled.", NotifyType.ErrorMessage);
                        break;

                    case StorePurchaseStatus.NetworkError:
                        rootPage.NotifyUser("Product was not purchased due to a Network Error.", NotifyType.ErrorMessage);
                        break;

                    case StorePurchaseStatus.ServerError:
                        rootPage.NotifyUser("Product was not purchased due to a Server Error.", NotifyType.ErrorMessage);
                        break;

                    default:
                        rootPage.NotifyUser("Product was not purchased due to an Unknown Error.", NotifyType.ErrorMessage);
                        break;
                }
            }
            else
            {
                rootPage.NotifyUser("You already bought this app and have a fully-licensed version.", NotifyType.ErrorMessage);
            }
        }
    }
}
