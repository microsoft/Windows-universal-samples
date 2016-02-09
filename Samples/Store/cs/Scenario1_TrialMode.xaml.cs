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
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// A page for first scenario.
    /// </summary>
    public sealed partial class Scenario1_TrialMode : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario1_TrialMode()
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
            CurrentAppSimulator.LicenseInformation.LicenseChanged += OnLicenseInformationChanged;
            await MainPage.ConfigureSimulatorAsync("trial-mode.xml");

            try
            {
                ListingInformation listing = await CurrentAppSimulator.LoadListingInformationAsync();
                PurchasePrice.Text = listing.FormattedPrice;
            }
            catch (Exception)
            {
                rootPage.NotifyUser("LoadListingInformationAsync API call failed", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Invoked when this page is about to unload
        /// </summary>
        /// <param name="e"></param>
        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            CurrentAppSimulator.LicenseInformation.LicenseChanged -= OnLicenseInformationChanged;
            base.OnNavigatingFrom(e);
        }

        /// <summary>
        /// Invoked when the licensing information changes.
        /// </summary>
        private void OnLicenseInformationChanged()
        {
            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                LicenseInformation licenseInformation = CurrentAppSimulator.LicenseInformation;
                if (licenseInformation.IsActive)
                {
                    if (licenseInformation.IsTrial)
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
            });
        }

        /// <summary>
        /// Invoked when the user asks to see trial period information.
        /// </summary>
        private void ShowTrialPeriodInformation()
        {
            LicenseInformation licenseInformation = CurrentAppSimulator.LicenseInformation;
            if (licenseInformation.IsActive)
            {
                if (licenseInformation.IsTrial)
                {
                    var remainingTrialTime = (licenseInformation.ExpirationDate - DateTime.Now).Days;
                    rootPage.NotifyUser("You can use this app for " + remainingTrialTime + " more days before the trial period ends.", NotifyType.StatusMessage);
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
            LicenseInformation licenseInformation = CurrentAppSimulator.LicenseInformation;
            rootPage.NotifyUser("Buying the full license...", NotifyType.StatusMessage);
            if (licenseInformation.IsTrial)
            {
                try
                {
                    await CurrentAppSimulator.RequestAppPurchaseAsync(false);
                    if (!licenseInformation.IsTrial && licenseInformation.IsActive)
                    {
                        rootPage.NotifyUser("You successfully upgraded your app to the fully-licensed version.", NotifyType.StatusMessage);
                    }
                    else
                    {
                        rootPage.NotifyUser("You still have a trial license for this app.", NotifyType.ErrorMessage);
                    }
                }
                catch (Exception)
                {
                    rootPage.NotifyUser("The upgrade transaction failed. You still have a trial license for this app.", NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("You already bought this app and have a fully-licensed version.", NotifyType.ErrorMessage);
            }
        }
    }
}
