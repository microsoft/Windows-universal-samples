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
using Windows.Graphics.Imaging;
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario4_BitmapReceipt : Page
    {
        private static MainPage rootPage;

        PosPrinter printer = null;
        ClaimedPosPrinter claimedPrinter = null;
        bool IsAnImportantTransaction = true;

        public Scenario4_BitmapReceipt()
        {
            this.InitializeComponent();
        }

        private void ResetTheScenarioState()
        {
            //Remove releasedevicerequested handler and dispose claimed printer object.
            if (claimedPrinter != null)
            {
                claimedPrinter.ReleaseDeviceRequested -= ClaimedPrinter_ReleaseDeviceRequested;
                claimedPrinter.Dispose();
                claimedPrinter = null;
            }

            if (printer != null)
            {
                printer.Dispose();
                printer = null;
            }
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            ResetTheScenarioState();
        }

        /// <summary>
        /// Invoked immediately before the Page is unloaded and is no longer the current source of a parent Frame
        /// </summary>
        /// <param name="e">Provides data for the OnNavigatingFrom callback that can be used to cancel a navigation 
        /// request from origination</param>
        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            ResetTheScenarioState();
        }

        /// <summary>
        /// Creates multiple tasks, first to find a receipt printer, then to claim the printer and then to enable and add releasedevicerequested event handler.
        /// </summary>
        async void FindClaimEnable_Click(object sender, RoutedEventArgs e)
        {
            if (await FindReceiptPrinter())
            {
                if (await ClaimPrinter())
                {
                    await EnableAsync();
                }
            }
        }

        void IsImportantTransaction_Click(object sender, RoutedEventArgs e)
        {
            var checkBox = (CheckBox)sender;
            IsAnImportantTransaction = checkBox.IsChecked.Value;
        }

        /// <summary>
        /// Default checkbox selection makes it an important transaction, hence we do not release claim when we get a release devicerequested event.
        /// </summary>
        async void ClaimedPrinter_ReleaseDeviceRequested(ClaimedPosPrinter sender, PosPrinterReleaseDeviceRequestedEventArgs args)
        {
            if (IsAnImportantTransaction)
            {
                await sender.RetainDeviceAsync();
            }
            else
            {
                ResetTheScenarioState();
            }
        }

        /// <summary>
        /// Prints the logo image.
        /// </summary>
        async void PrintBitmap_Click(object sender, RoutedEventArgs e)
        {
            await PrintBitmap();
        }

        private void EndScenario_Click(object sender, RoutedEventArgs e)
        {
            ResetTheScenarioState();
            rootPage.NotifyUser("Disconnected Printer", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Actual claim method task that claims the printer asynchronously
        /// </summary>
        private async Task<bool> ClaimPrinter()
        {
            if (claimedPrinter == null)
            {
                claimedPrinter = await printer.ClaimPrinterAsync();
                if (claimedPrinter != null)
                {
                    claimedPrinter.ReleaseDeviceRequested += ClaimedPrinter_ReleaseDeviceRequested;
                    rootPage.NotifyUser("Claimed Printer", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("Claim Printer failed", NotifyType.ErrorMessage);
                    return false;
                }
            }
            return true;
        }

        /// <summary>
        /// Enable the claimed printer.
        /// </summary>
        private async Task<bool> EnableAsync()
        {
            if (claimedPrinter == null)
            {
                rootPage.NotifyUser("No Claimed Printer to enable", NotifyType.ErrorMessage);
                return false;
            }

            if (!await claimedPrinter.EnableAsync())
            {
                rootPage.NotifyUser("Could not enable printer", NotifyType.ErrorMessage);
                return false;
            }

            rootPage.NotifyUser("Enabled printer.", NotifyType.StatusMessage);

            return true;
        }

        private async Task<bool> PrintBitmap()
        {
            if (!IsPrinterClaimed())
            {
                return false;
            }
            claimedPrinter.Receipt.IsLetterQuality = true;
            ReceiptPrintJob job = claimedPrinter.Receipt.CreateJob();
            BitmapFrame logoFrame = await LoadLogoBitmapAsync();
            job.PrintBitmap(logoFrame, PosPrinterAlignment.Center);
            if (await job.ExecuteAsync())
            {
                rootPage.NotifyUser("Bitmap printed successfully", NotifyType.StatusMessage);
                return true;
            }
            rootPage.NotifyUser("Was not able to print bitmap", NotifyType.ErrorMessage);
            return false;
        }

        public static async Task<BitmapFrame> LoadLogoBitmapAsync()
        {
            var uri = new Uri("ms-appx:///Assets/coffee-logo.png");
            StorageFile file = await StorageFile.GetFileFromApplicationUriAsync(uri);
            using (var readStream = await file.OpenReadAsync())
            {
                BitmapDecoder decoder = await BitmapDecoder.CreateAsync(readStream);
                return await decoder.GetFrameAsync(0);
            }
        }

        /// <summary>
        /// Check to make sure we still have claim on printer
        /// </summary>
        private bool IsPrinterClaimed()
        {
            if (printer == null)
            {
                rootPage.NotifyUser("Need to find printer first", NotifyType.ErrorMessage);
                return false;
            }

            if (claimedPrinter == null)
            {
                rootPage.NotifyUser("No claimed printer.", NotifyType.ErrorMessage);
                return false;
            }

            if (claimedPrinter.Receipt == null)
            {
                rootPage.NotifyUser("No receipt printer object in claimed printer.", NotifyType.ErrorMessage);
                return false;
            }

            return true;
        }

        private async Task<bool> FindReceiptPrinter()
        {
            if (printer == null)
            {
                rootPage.NotifyUser("Finding printer", NotifyType.StatusMessage);
                printer = await DeviceHelpers.GetFirstReceiptPrinterAsync();
                if (printer != null)
                {
                    rootPage.NotifyUser("Got Printer with Device Id : " + printer.DeviceId, NotifyType.StatusMessage);
                    return true;
                }
                else
                {
                    rootPage.NotifyUser("No Printer found", NotifyType.ErrorMessage);
                    return false;
                }
            }
            return true;
        }
    }
}
