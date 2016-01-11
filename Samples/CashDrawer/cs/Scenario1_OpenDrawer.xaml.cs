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
using Windows.Devices.PointOfService;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1_OpenDrawer : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        CashDrawer drawer = null;
        ClaimedCashDrawer claimedDrawer = null;


        public Scenario1_OpenDrawer()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            ResetScenarioState();
            base.OnNavigatedTo(e);
        }

        /// <summary>
        /// Invoked before the page is unloaded and is no longer the current source of a Frame.
        /// </summary>
        /// <param name="e">Event data describing the navigation that was requested.</param>
        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            ResetScenarioState();
            base.OnNavigatedFrom(e);
        }

        /// <summary>
        /// Event handler for Initialize Drawer button.
        /// Claims and enables the default cash drawer.
        /// </summary>
        /// <param name="sender">Button that was clicked.</param>
        /// <param name="e">Event data associated with click event.</param>
        private async void InitDrawerButton_Click(object sender, RoutedEventArgs e)
        {
            if (await CreateDefaultCashDrawerObject())
            {
                if (await ClaimCashDrawer())
                {
                    if (await EnableCashDrawer())
                    {
                        InitDrawerButton.IsEnabled = false;
                        OpenDrawerButton.IsEnabled = true;

                        rootPage.NotifyUser("Successfully enabled cash drawer. Device ID: " + claimedDrawer.DeviceId, NotifyType.StatusMessage);
                    }
                    else
                    {
                        rootPage.NotifyUser("Failed to enable cash drawer.", NotifyType.ErrorMessage);
                    }
                }
                else
                {
                    rootPage.NotifyUser("Failed to claim cash drawer.", NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("Cash drawer not found. Please connect a cash drawer.", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Open the cash drawer.
        /// </summary>
        /// <param name="sender">Button that was clicked.</param>
        /// <param name="e">Event data associated with click event.</param>
        private async void OpenDrawerButton_Click(object sender, RoutedEventArgs e)
        {
            if (await OpenCashDrawer())
                rootPage.NotifyUser("Cash drawer open succeeded.", NotifyType.StatusMessage);
            else
                rootPage.NotifyUser("Cash drawer open failed.", NotifyType.ErrorMessage);
        }

        /// <summary>
        /// Creates the default cash drawer.
        /// </summary>
        /// <returns>True if the cash drawer was created, false otherwise.</returns>
        private async Task<bool> CreateDefaultCashDrawerObject()
        {
            rootPage.NotifyUser("Creating cash drawer object.", NotifyType.StatusMessage);

            if (drawer == null)
            {
                drawer = await CashDrawer.GetDefaultAsync();
                if (drawer == null)
                    return false;
            }

            return true;
        }

        /// <summary>
        /// Attempt to claim the connected cash drawer.
        /// </summary>
        /// <returns>True if the cash drawer was successfully claimed, false otherwise.</returns>
        private async Task<bool> ClaimCashDrawer()
        {
            if (drawer == null)
                return false;

            if (claimedDrawer == null)
            {
                claimedDrawer = await drawer.ClaimDrawerAsync();
                if (claimedDrawer == null)
                    return false;
            }

            return true;
        }

        /// <summary>
        /// Attempt to enabled the claimed cash drawer.
        /// </summary>
        /// <returns>True if the cash drawer was successfully enabled, false otherwise.</returns>
        private async Task<bool> EnableCashDrawer()
        {
            if (claimedDrawer == null)
                return false;

            if (claimedDrawer.IsEnabled)
                return true;

            return await claimedDrawer.EnableAsync();
        }

        /// <summary>
        /// Attempt to open the claimed cash drawer.
        /// </summary>
        /// <returns>True if the cash drawer was successfully opened, false otherwise.</returns>
        private async Task<bool> OpenCashDrawer()
        {
            if (claimedDrawer == null || !claimedDrawer.IsEnabled)
                return false;

            return await claimedDrawer.OpenDrawerAsync();
        }

        /// <summary>
        /// Reset the scenario to its initial state.
        /// </summary>
        private void ResetScenarioState()
        {
            drawer = null;

            if (claimedDrawer != null)
            {
                claimedDrawer.Dispose();
                claimedDrawer = null;
            }

            InitDrawerButton.IsEnabled = true;
            OpenDrawerButton.IsEnabled = false;

            rootPage.NotifyUser("Click the init drawer button to begin.", NotifyType.StatusMessage);
        }
    }
}
