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
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// A page for third scenario.
    /// </summary>
    public sealed partial class Scenario3_ExpiringProduct : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario3_ExpiringProduct()
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
            CurrentAppSimulator.LicenseInformation.LicenseChanged += ExpiringProductRefreshScenario;
            await MainPage.ConfigureSimulatorAsync("expiring-product.xml");
        }

        /// <summary>
        /// Invoked when this page is about to unload
        /// </summary>
        /// <param name="e"></param>
        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            CurrentAppSimulator.LicenseInformation.LicenseChanged -= ExpiringProductRefreshScenario;
            base.OnNavigatingFrom(e);
        }

        private void ExpiringProductRefreshScenario()
        {
            var task = Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                LicenseInformation licenseInformation = CurrentAppSimulator.LicenseInformation;
                if (licenseInformation.IsActive)
                {
                    if (licenseInformation.IsTrial)
                    {
                        rootPage.NotifyUser("You don't have a full license to this app.", NotifyType.ErrorMessage);
                    }
                    else
                    {
                        var productLicense1 = licenseInformation.ProductLicenses["product1"];
                        if (productLicense1.IsActive)
                        {
                            var longdateTemplate = new Windows.Globalization.DateTimeFormatting.DateTimeFormatter("longdate");
                            Product1Message.Text = "Product 1 expires on " + longdateTemplate.Format(productLicense1.ExpirationDate) + ". Days remaining: " + (productLicense1.ExpirationDate - DateTime.Now).Days;
                        }
                        else
                        {
                            rootPage.NotifyUser("Product 1 is not available.", NotifyType.ErrorMessage);
                        }
                    }
                }
                else
                {
                    rootPage.NotifyUser("You don't have active license.", NotifyType.ErrorMessage);
                }
            });
        }
    }
}
