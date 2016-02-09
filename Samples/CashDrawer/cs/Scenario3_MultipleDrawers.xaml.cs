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
    public sealed partial class Scenario3_MultipleDrawers : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        CashDrawer cashDrawerInstance1 = null;
        CashDrawer cashDrawerInstance2 = null;
        ClaimedCashDrawer claimedCashDrawerInstance1 = null;
        ClaimedCashDrawer claimedCashDrawerInstance2 = null;


        public Scenario3_MultipleDrawers()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Enumerator for current active Instance.
        /// </summary>
        private enum CashDrawerInstance
        {
            Instance1,
            Instance2
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
        /// Creates the default cash drawer.
        /// </summary>
        /// <param name="instance">Specifies the cash drawer instance that should be used.</param>
        /// <returns>True if the cash drawer was created, false otherwise.</returns>
        private async Task<bool> CreateDefaultCashDrawerObject(CashDrawerInstance instance)
        {
            rootPage.NotifyUser("Creating cash drawer object.", NotifyType.StatusMessage);

            CashDrawer tempDrawer = null;
            tempDrawer = await CashDrawer.GetDefaultAsync();

            if (tempDrawer == null)
            {
                rootPage.NotifyUser("Cash drawer not found. Please connect a cash drawer.", NotifyType.ErrorMessage);
                return false;
            }

            switch (instance)
            {
                case CashDrawerInstance.Instance1:
                    cashDrawerInstance1 = tempDrawer;
                    break;
                case CashDrawerInstance.Instance2:
                    cashDrawerInstance2 = tempDrawer;
                    break;
                default:
                    return false;
            }

            return true;
        }

        /// <summary>
        /// Attempt to claim the connected cash drawer.
        /// </summary>
        /// <param name="instance">Specifies the cash drawer instance that should be used.</param>
        /// <returns>True if the cash drawer was successfully claimed, false otherwise.</returns>
        private async Task<bool> ClaimCashDrawer(CashDrawerInstance instance)
        {
            bool claimAsyncStatus = false;

            switch (instance)
            {
                case CashDrawerInstance.Instance1:

                    claimedCashDrawerInstance1 = await cashDrawerInstance1.ClaimDrawerAsync();
                    if (claimedCashDrawerInstance1 != null)
                    {
                        rootPage.NotifyUser("Successfully claimed instance 1.", NotifyType.StatusMessage);
                        claimAsyncStatus = true;
                    }
                    else
                    {
                        rootPage.NotifyUser("Failed to claim instance 1.", NotifyType.ErrorMessage);
                    }
                    break;

                case CashDrawerInstance.Instance2:

                    claimedCashDrawerInstance2 = await cashDrawerInstance2.ClaimDrawerAsync();
                    if (claimedCashDrawerInstance2 != null)
                    {
                        rootPage.NotifyUser("Successfully claimed instance 2.", NotifyType.StatusMessage);
                        claimAsyncStatus = true;
                    }
                    else
                    {
                        rootPage.NotifyUser("Failed to claim instance 2.", NotifyType.ErrorMessage);
                    }
                    break;

                default:
                    break;
            }

            return claimAsyncStatus;
        }

        /// <summary>
        /// Event callback for claim instance 1 button.
        /// </summary>
        /// <param name="sender">Button that was clicked.</param>
        /// <param name="e">Event data associated with click event.</param>
        private async void ClaimButton1_Click(object sender, RoutedEventArgs e)
        {
            if (await CreateDefaultCashDrawerObject(CashDrawerInstance.Instance1))
            {
                if (await ClaimCashDrawer(CashDrawerInstance.Instance1))
                {
                    claimedCashDrawerInstance1.ReleaseDeviceRequested += claimedCashDrawerInstance1_ReleaseDeviceRequested;
                    SetClaimedUI(CashDrawerInstance.Instance1);
                }
                else
                {
                    cashDrawerInstance2 = null;
                }
            }
        }

        /// <summary>
        /// Event callback for claim instance 2 button.
        /// </summary>
        /// <param name="sender">Button that was clicked.</param>
        /// <param name="e">Event data associated with click event.</param>
        private async void ClaimButton2_Click(object sender, RoutedEventArgs e)
        {
            if (await CreateDefaultCashDrawerObject(CashDrawerInstance.Instance2))
            {
                if (await ClaimCashDrawer(CashDrawerInstance.Instance2))
                {
                    claimedCashDrawerInstance2.ReleaseDeviceRequested += claimedCashDrawerInstance2_ReleaseDeviceRequested;
                    SetClaimedUI(CashDrawerInstance.Instance2);
                }
                else
                {
                    cashDrawerInstance2 = null;
                }
            }
        }

        /// <summary>
        /// Event callback for release instance 1 button.
        /// </summary>
        /// <param name="sender">Button that was clicked.</param>
        /// <param name="e">Event data associated with click event.</param>
        private void ReleaseButton1_Click(object sender, RoutedEventArgs e)
        {
            if (claimedCashDrawerInstance1 != null)
            {
                claimedCashDrawerInstance1.Dispose();
                claimedCashDrawerInstance1 = null;

                SetReleasedUI(CashDrawerInstance.Instance1);

                rootPage.NotifyUser("Claimed instance 1 was released.", NotifyType.StatusMessage);
            }
        }

        /// <summary>
        /// Event callback for release instance 2 button.
        /// </summary>
        /// <param name="sender">Button that was clicked.</param>
        /// <param name="e">Event data associated with click event.</param>
        private void ReleaseButton2_Click(object sender, RoutedEventArgs e)
        {
            if (claimedCashDrawerInstance2 != null)
            {
                claimedCashDrawerInstance2.Dispose();
                claimedCashDrawerInstance2 = null;

                SetReleasedUI(CashDrawerInstance.Instance2);

                rootPage.NotifyUser("Claimed instance 2 was released.", NotifyType.StatusMessage);
            }
        }

        /// <summary>
        /// Event callback for a release device request for instance 1.
        /// </summary>
        /// <param name="sender">The drawer receiving the release device request.</param>
        /// <param name="e">Unused for ReleaseDeviceRequested events.</param>
        private async void claimedCashDrawerInstance1_ReleaseDeviceRequested(ClaimedCashDrawer sender, object e)
        {
            await MainPage.Current.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
            {
                rootPage.NotifyUser("Release instance 1 requested.", NotifyType.StatusMessage);

                if (RetainCheckBox1.IsChecked == true)
                {
                    if (await claimedCashDrawerInstance1.RetainDeviceAsync() == false)
                        rootPage.NotifyUser("Cash drawer instance 1 retain failed.", NotifyType.ErrorMessage);
                }
                else
                {
                    claimedCashDrawerInstance1.Dispose();
                    claimedCashDrawerInstance1 = null;

                    SetReleasedUI(CashDrawerInstance.Instance1);
                }
            });
        }

        /// <summary>
        /// Event callback for a release device request for instance 2.
        /// </summary>
        /// <param name="sender">The drawer receiving the release device request.</param>
        /// <param name="e">Unused for ReleaseDeviceRequested events.</param>
        private async void claimedCashDrawerInstance2_ReleaseDeviceRequested(ClaimedCashDrawer drawer, object e)
        {
            await MainPage.Current.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
            {
                rootPage.NotifyUser("Release instance 2 requested.", NotifyType.StatusMessage);

                if (RetainCheckBox2.IsChecked == true)
                {
                    if (await claimedCashDrawerInstance2.RetainDeviceAsync() == false)
                        rootPage.NotifyUser("Cash drawer instance 2 retain failed.", NotifyType.ErrorMessage);
                }
                else
                {
                    claimedCashDrawerInstance2.Dispose();
                    claimedCashDrawerInstance2 = null;

                    SetReleasedUI(CashDrawerInstance.Instance2);
                }
            });
        }

        /// <summary>
        /// Resets the display elements to original state
        /// </summary>
        private void ResetScenarioState()
        {
            cashDrawerInstance1 = null;
            cashDrawerInstance2 = null;

            if (claimedCashDrawerInstance1 != null)
            {
                claimedCashDrawerInstance1.Dispose();
                claimedCashDrawerInstance1 = null;
            }

            if (claimedCashDrawerInstance2 != null)
            {
                claimedCashDrawerInstance2.Dispose();
                claimedCashDrawerInstance2 = null;
            }

            ClaimButton1.IsEnabled = true;
            ClaimButton2.IsEnabled = true;
            ReleaseButton1.IsEnabled = false;
            ReleaseButton2.IsEnabled = false;
            RetainCheckBox1.IsChecked = true;
            RetainCheckBox2.IsChecked = true;

            rootPage.NotifyUser("Click a claim button to begin.", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Sets the UI elements to a state corresponding to the specified instace being claimed.
        /// </summary>
        /// <param name="instance">Corresponds to instance that has been claimed.</param>
        private void SetClaimedUI(CashDrawerInstance instance)
        {
            switch (instance)
            {
                case CashDrawerInstance.Instance1:
                    ClaimButton1.IsEnabled = false;
                    ClaimButton2.IsEnabled = true;
                    ReleaseButton1.IsEnabled = true;
                    ReleaseButton2.IsEnabled = false;
                    break;

                case CashDrawerInstance.Instance2:
                    ClaimButton1.IsEnabled = true;
                    ClaimButton2.IsEnabled = false;
                    ReleaseButton1.IsEnabled = false;
                    ReleaseButton2.IsEnabled = true;
                    break;

                default:
                    break;
            }
        }

        /// <summary>
        /// Sets the UI elements to a state corresponding to the specified instace being released.
        /// </summary>
        /// <param name="instance">Corresponds to instance that has been released.</param>
        private void SetReleasedUI(CashDrawerInstance instance)
        {
            switch (instance)
            {
                case CashDrawerInstance.Instance1:
                    ClaimButton1.IsEnabled = true;
                    ReleaseButton1.IsEnabled = false;
                    break;

                case CashDrawerInstance.Instance2:
                    ClaimButton2.IsEnabled = true;
                    ReleaseButton2.IsEnabled = false;
                    break;

                default:
                    break;
            }
        }
    }
}
