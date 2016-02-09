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
    public sealed partial class Scenario2_CloseDrawer : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        CashDrawer drawer = null;
        ClaimedCashDrawer claimedDrawer = null;


        public Scenario2_CloseDrawer()
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
                        drawer.StatusUpdated += drawer_StatusUpdated;

                        InitDrawerButton.IsEnabled = false;
                        DrawerWaitButton.IsEnabled = true;

                        UpdateStatusOutput(drawer.Status.StatusKind);
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
        /// Set up an alarm and wait for the drawer to close.
        /// </summary>
        /// <param name="sender">Button that was clicked.</param>
        /// <param name="e">Event data associated with click event.</param>
        private async void WaitForDrawerCloseButton_Click(object sender, RoutedEventArgs e)
        {
            if (claimedDrawer == null)
            {
                rootPage.NotifyUser("Drawer must be initialized.", NotifyType.ErrorMessage);
                return;
            }

            var alarm = claimedDrawer.CloseAlarm;
            if (alarm == null)
            {
                rootPage.NotifyUser("Failed to create drawer alarm.", NotifyType.ErrorMessage);
                return;
            }

            // TimeSpan specifies a time period as (hours, minutes, seconds)
            alarm.AlarmTimeout = new TimeSpan(0, 0, 30);
            alarm.BeepDelay = new TimeSpan(0, 0, 3);
            alarm.BeepDuration = new TimeSpan(0, 0, 1);
            alarm.BeepFrequency = 700;
            alarm.AlarmTimeoutExpired += drawer_AlarmExpired;

            rootPage.NotifyUser("Waiting for drawer to close.", NotifyType.StatusMessage);

            if (await alarm.StartAsync())
                rootPage.NotifyUser("Successfully waited for drawer close.", NotifyType.StatusMessage);
            else
                rootPage.NotifyUser("Failed to wait for drawer close.", NotifyType.ErrorMessage);
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
        /// Event callback for device status updates.
        /// </summary>
        /// <param name="drawer">CashDrawer object sending the status update event.</param>
        /// <param name="e">Event data associated with the status update.</param>
        private async void drawer_StatusUpdated(CashDrawer drawer, CashDrawerStatusUpdatedEventArgs e)
        {
            await MainPage.Current.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                UpdateStatusOutput(e.Status.StatusKind);
                rootPage.NotifyUser("Status updated event: " + e.Status.StatusKind.ToString(), NotifyType.StatusMessage);
            });
        }

        /// <summary>
        /// Event callback for the alarm expiring.
        /// </summary>
        /// <param name="alarm">CashDrawerCloseAlarm that has expired.</param>
        /// <param name="sender">Unused by AlarmTimeoutExpired events.</param>
        private async void drawer_AlarmExpired(CashDrawerCloseAlarm alarm, object sender)
        {
            await MainPage.Current.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                rootPage.NotifyUser("Alarm expired.  Still waiting for drawer to be closed.", NotifyType.StatusMessage);
            });
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

            UpdateStatusOutput(CashDrawerStatusKind.Offline);

            InitDrawerButton.IsEnabled = true;
            DrawerWaitButton.IsEnabled = false;

            rootPage.NotifyUser("Click the init drawer button to begin.", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Update the cash drawer text block.
        /// </summary>
        /// <param name="status">Cash drawer status to be displayed.</param>
        private void UpdateStatusOutput(CashDrawerStatusKind status)
        {
            DrawerStatusBlock.Text = status.ToString();
        }
    }
}
