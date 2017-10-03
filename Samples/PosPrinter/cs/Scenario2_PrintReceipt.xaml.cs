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

namespace SDKTemplate
{
    public sealed partial class Scenario2_PrintReceipt : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario2_PrintReceipt()
        {
            this.InitializeComponent();
        }

        bool IsPrinterClaimed()
        {
            if (rootPage.ClaimedPrinter != null)
            {
                rootPage.NotifyUser("", NotifyType.StatusMessage);
                return true;
            }
            else
            {
                rootPage.NotifyUser("Use scenario 1 to find, claim, and enable a receipt printer.", NotifyType.ErrorMessage);
                return false;
            }
        }


        async Task<bool> ExecuteJobAndReportResultAsync(ReceiptPrintJob job)
        {
            bool success = await job.ExecuteAsync();
            if (success)
            {
                rootPage.NotifyUser("Printing complete.", NotifyType.StatusMessage);
            }
            else
            {

                ClaimedReceiptPrinter receiptPrinter = rootPage.ClaimedPrinter.Receipt;
                string reason;
                if (receiptPrinter.IsCartridgeEmpty)
                {
                    reason = "Printer is out of ink. Please replace cartridge.";
                }
                else if (receiptPrinter.IsCartridgeRemoved)
                {
                    reason = "Printer cartridge is missing. Please replace cartridge.";
                }
                else if (receiptPrinter.IsCoverOpen)
                {
                    reason = "Printer cover is open. Please close it.";
                }
                else if (receiptPrinter.IsHeadCleaning)
                {
                    reason = "Printer is currently cleaning the cartridge. Please wait until cleaning has completed.";
                }
                else if (receiptPrinter.IsPaperEmpty)
                {
                    reason = "Printer is out of paper. Please insert a new roll.";
                }
                else
                {
                    reason = "Unable to print.";
                }
                rootPage.NotifyUser(reason, NotifyType.ErrorMessage);
            }
            return success;
        }

        #region Print a single line of text
        /// <summary>
        /// Prints the line that is in the textbox.
        /// </summary>
        async void PrintLine_Click()
        {
            if (!IsPrinterClaimed())
            {
                return;
            }

            ReceiptPrintJob job = rootPage.ClaimedPrinter.Receipt.CreateJob();
            job.PrintLine(PrintLineTextBox.Text);
            await ExecuteJobAndReportResultAsync(job);
        }
        #endregion

        #region Print a sample receipt
        async void PrintReceipt_Click()
        {
            if (!IsPrinterClaimed())
            {
                return;
            }

            string receiptString = "======================\n" +
                                   "|   Sample Header    |\n" +
                                   "======================\n" +
                                   "Item             Price\n" +
                                   "----------------------\n" +
                                   "Books            10.40\n" +
                                   "Games             9.60\n" +
                                   "----------------------\n" +
                                   "Total            20.00\n";

            rootPage.NotifyUser("Printing receipt...", NotifyType.StatusMessage);

            // The job consists of two receipts, one for the merchant and one
            // for the customer.
            ReceiptPrintJob job = rootPage.ClaimedPrinter.Receipt.CreateJob();
            PrintLineFeedAndCutPaper(job, receiptString + GetMerchantFooter());
            PrintLineFeedAndCutPaper(job, receiptString + GetCustomerFooter());

            await ExecuteJobAndReportResultAsync(job);
        }

        string GetMerchantFooter()
        {
            return "\n" +
                   "______________________\n" +
                   "Signature\n" +
                   "\n" +
                   "Merchant Copy\n";
        }

        string GetCustomerFooter()
        {
            return "\n" +
                   "Customer Copy\n";
        }

        // Cut the paper after printing enough blank lines to clear the paper cutter.
        private void PrintLineFeedAndCutPaper(ReceiptPrintJob job, string receipt)
        {
            // Passing a multi-line string to the Print method results in
            // smoother paper feeding than sending multiple single-line strings
            // to PrintLine.
            string feedString = "";
            for (uint n = 0; n < rootPage.ClaimedPrinter.Receipt.LinesToPaperCut; n++)
            {
                feedString += "\n";
            }
            job.Print(receipt + feedString);
            if (rootPage.Printer.Capabilities.Receipt.CanCutPaper)
            {
                job.CutPaper();
            }
        }
        #endregion

        #region Print a bitmap
        private async void PrintBitmap_Click()
        {
            if (!IsPrinterClaimed())
            {
                return;
            }
            rootPage.ClaimedPrinter.Receipt.IsLetterQuality = true;
            ReceiptPrintJob job = rootPage.ClaimedPrinter.Receipt.CreateJob();
            BitmapFrame logoFrame = await LoadLogoBitmapAsync();
            job.PrintBitmap(logoFrame, PosPrinterAlignment.Center);
            await ExecuteJobAndReportResultAsync(job);
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
        #endregion

        #region Print a barcode
        private async void PrintBarcode_Click()
        {
            if (!IsPrinterClaimed())
            {
                return;
            }

            rootPage.ClaimedPrinter.Receipt.IsLetterQuality = true;
            ReceiptPrintJob job = rootPage.ClaimedPrinter.Receipt.CreateJob();
            job.PrintBarcode(BarcodeText.Text, BarcodeSymbologies.Upca, 60, 3, PosPrinterBarcodeTextPosition.Below, PosPrinterAlignment.Center);
            await ExecuteJobAndReportResultAsync(job);
        }
        #endregion
    }
}
