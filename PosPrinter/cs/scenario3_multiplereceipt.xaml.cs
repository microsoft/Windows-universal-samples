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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Devices.PointOfService;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using SDKTemplate;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace PosPrinterSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario3_MultipleReceipt : Page
    {
        // Uncomment the following line if you need a pointer back to the MainPage.
        // This value should be obtained in OnNavigatedTo in the e.Parameter argument
        private MainPage rootPage;

        PosPrinter printerInstance1 = null;
        PosPrinter printerInstance2 = null;
        ClaimedPosPrinter claimedPrinter1 = null;
        ClaimedPosPrinter claimedPrinter2 = null;
        bool IsAnImportantTransactionInstance1;
        bool IsAnImportantTransactionInstance2;

        public Scenario3_MultipleReceipt()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            ResetTheScenarioState();
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            ResetTheScenarioState();
        }

        async void FindReceiptPrinter_Click(object sender, RoutedEventArgs e)
        {
            await FindReceiptPrinterInstances();
        }

        /// <summary>
        /// Claims printer instance 1
        /// </summary>
        async void Claim1_Click(object sender, RoutedEventArgs e)
        {
            await ClaimAndEnablePrinter1();
        }

        /// <summary>
        /// Releases claim of printer instance 1
        /// </summary>
        void Release1_Click(object sender, RoutedEventArgs e)
        {
            if (claimedPrinter1 != null)
            {
                claimedPrinter1.ReleaseDeviceRequested -= ClaimedPrinter1_ReleaseDeviceRequested;
                claimedPrinter1.Dispose();
                claimedPrinter1 = null;
                rootPage.NotifyUser("Released claimed Instance 1", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("Instance 1 not claimed to release", NotifyType.StatusMessage);
            }
        }

        /// <summary>
        /// Claims printer instance 2
        /// </summary>
        async void Claim2_Click(object sender, RoutedEventArgs e)
        {
            await ClaimAndEnablePrinter2();
        }

        /// <summary>
        /// Releases claim of printer instance 2
        /// </summary>
        void Release2_Click(object sender, RoutedEventArgs e)
        {
            if (claimedPrinter2 != null)
            {
                claimedPrinter2.ReleaseDeviceRequested -= ClaimedPrinter2_ReleaseDeviceRequested;
                claimedPrinter2.Dispose();
                claimedPrinter2 = null;
                rootPage.NotifyUser("Released claimed Instance 2", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("Instance 2 not claimed to release", NotifyType.StatusMessage);
            }
        }

        /// <summary>
        ///Actual claim method task that claims and enables printer instance 1 asynchronously. It then adds the releasedevicerequested event handler as well to the claimed printer.
        /// </summary>
        private async Task<bool> ClaimAndEnablePrinter1()
        {
            if (printerInstance1 == null)
            {
                rootPage.NotifyUser("Cound not claim printer. Make sure you find printer first.", NotifyType.ErrorMessage);
                return false;
            }

            if (claimedPrinter1 == null)
            {
                claimedPrinter1 = await printerInstance1.ClaimPrinterAsync();
                if (claimedPrinter1 != null)
                {
                    claimedPrinter1.ReleaseDeviceRequested += ClaimedPrinter1_ReleaseDeviceRequested;
                    rootPage.NotifyUser("Claimed Printer Instance 1", NotifyType.StatusMessage);
                    if (!await claimedPrinter1.EnableAsync())
                    {
                        rootPage.NotifyUser("Could not enable printer instance 1.", NotifyType.ErrorMessage);
                    }
                    else
                    {
                        return true;
                    }
                }
                else
                {
                    rootPage.NotifyUser("Claim Printer instance 1 failed. Probably because instance 2 is holding on to it.", NotifyType.ErrorMessage);
                }
            }
            return false;
        }

        /// <summary>
        ///Actual claim method task that claims and enables printer instance 2 asynchronously. It then adds the releasedevicerequested event handler as well to the claimed printer.
        /// </summary>
        private async Task<bool> ClaimAndEnablePrinter2()
        {
            if (printerInstance2 == null)
            {
                rootPage.NotifyUser("Cound not claim printer. Make sure you find printer first.", NotifyType.ErrorMessage);
                return false;
            }

            if (printerInstance1 != null && claimedPrinter2 == null)
            {
                rootPage.NotifyUser("Trying to claim instance 2.", NotifyType.StatusMessage);

                claimedPrinter2 = await printerInstance2.ClaimPrinterAsync();
                if (claimedPrinter2 != null)
                {
                    claimedPrinter2.ReleaseDeviceRequested += ClaimedPrinter2_ReleaseDeviceRequested;
                    rootPage.NotifyUser("Claimed Printer Instance 2", NotifyType.StatusMessage);
                    if (!await claimedPrinter2.EnableAsync())
                    {
                        rootPage.NotifyUser("Could not enable printer instance 2.", NotifyType.ErrorMessage);
                    }
                    else
                    {
                        return true;
                    }
                }
                else
                {
                    rootPage.NotifyUser("Claim Printer instance 2 failed. Probably because instance 1 is holding on to it.", NotifyType.ErrorMessage);
                }
            }
            return false;
        }

        /// <summary>
        /// PosPrinter GetDeviceSelector gets the string format used to search for pos printer. This is then used to find any pos printers.
        /// The method then takes the first printer id found and tries to create two instances for that printer.
        /// </summary>
        /// <returns></returns>
        private async Task<bool> FindReceiptPrinterInstances()
        {
            if (printerInstance1 == null || printerInstance2 == null)
            {
                rootPage.NotifyUser("Finding printer", NotifyType.StatusMessage);
                DeviceInformationCollection deviceCollection = await DeviceInformation.FindAllAsync(PosPrinter.GetDeviceSelector());
                if (deviceCollection != null && deviceCollection.Count > 0)
                {
                    DeviceInformation deviceInfo = deviceCollection[0];
                    printerInstance1 = await PosPrinter.FromIdAsync(deviceInfo.Id);
                    if (printerInstance1.Capabilities.Receipt.IsPrinterPresent)
                    {
                        rootPage.NotifyUser("Got Printer Instance 1 with Device Id : " + printerInstance1.DeviceId, NotifyType.StatusMessage);

                        printerInstance2 = await PosPrinter.FromIdAsync(deviceInfo.Id);
                        if (printerInstance2.Capabilities.Receipt.IsPrinterPresent)
                        {
                            rootPage.NotifyUser("Got Printer Instance 2 with Device Id : " + printerInstance2.DeviceId, NotifyType.StatusMessage);
                        }

                        return true;
                    }
                    else
                    {
                        rootPage.NotifyUser("Did not find a Receipt printer ", NotifyType.ErrorMessage);
                        return false;
                    }
                }
                else
                {
                    rootPage.NotifyUser("No devices returned by FindAllAsync.", NotifyType.ErrorMessage);
                    return false;
                }

            }

            return true;
        }

        /// <summary>
        /// Reset all instances of claimed printer and remove event handlers.
        /// </summary>
        private void ResetTheScenarioState()
        {
            IsAnImportantTransactionInstance1 = true;
            IsAnImportantTransactionInstance2 = false;
            chkInstance1.IsChecked = true;
            chkInstance2.IsChecked = false;

            //Remove releasedevicerequested handler and dispose claimed printer object.
            if (claimedPrinter1 != null)
            {
                claimedPrinter1.ReleaseDeviceRequested -= ClaimedPrinter1_ReleaseDeviceRequested;
                claimedPrinter1.Dispose();
                claimedPrinter1 = null;
            }

            if (claimedPrinter2 != null)
            {
                claimedPrinter2.ReleaseDeviceRequested -= ClaimedPrinter2_ReleaseDeviceRequested;
                claimedPrinter2.Dispose();
                claimedPrinter2 = null;
            }
            printerInstance1 = null;
            printerInstance2 = null;
        }

        async void ClaimedPrinter1_ReleaseDeviceRequested(ClaimedPosPrinter sender, PosPrinterReleaseDeviceRequestedEventArgs args)
        {
            if (IsAnImportantTransactionInstance1)
            {
                await sender.RetainDeviceAsync();
            }
            else
            {
                claimedPrinter1.ReleaseDeviceRequested -= ClaimedPrinter1_ReleaseDeviceRequested;
                claimedPrinter1.Dispose();
                claimedPrinter1 = null;
            }
        }

        async void ClaimedPrinter2_ReleaseDeviceRequested(ClaimedPosPrinter sender, PosPrinterReleaseDeviceRequestedEventArgs args)
        {
            if (IsAnImportantTransactionInstance2)
            {
                await sender.RetainDeviceAsync();
            }
            else
            {
                claimedPrinter2.ReleaseDeviceRequested -= ClaimedPrinter2_ReleaseDeviceRequested;
                claimedPrinter2.Dispose();
                claimedPrinter2 = null;
            }
        }

        async void PrintLine1_Click(object sender, RoutedEventArgs e)
        {
            await PrintLine(txtPrintLine1.Text, claimedPrinter1);
        }

        async void PrintLine2_Click(object sender, RoutedEventArgs e)
        {
            await PrintLine(txtPrintLine2.Text, claimedPrinter2);
        }
        private async Task<bool> PrintLine(string message, ClaimedPosPrinter claimedPrinter)
        {
            if (claimedPrinter == null)
            {
                rootPage.NotifyUser("Claimed printer object is null. Cannot print.", NotifyType.ErrorMessage);
                return false;
            }

            if (claimedPrinter.Receipt == null)
            {
                rootPage.NotifyUser("No receipt printer object in claimed printer.", NotifyType.ErrorMessage);
                return false;
            }

            ReceiptPrintJob job = claimedPrinter.Receipt.CreateJob();
            job.PrintLine(message);
            if (await job.ExecuteAsync())
            {
                rootPage.NotifyUser("Printed line", NotifyType.StatusMessage);
                return true;
            }
            rootPage.NotifyUser("Was not able to print line", NotifyType.StatusMessage);
            return false;
        }

        private void EndScenario_Click(object sender, RoutedEventArgs e)
        {
            ResetTheScenarioState();
            rootPage.NotifyUser("Disconnected Receipt Printer", NotifyType.StatusMessage);
        }

        private void chkInstance1_Checked(object sender, RoutedEventArgs e)
        {

            IsAnImportantTransactionInstance1 = true;
        }

        private void chkInstance2_Checked(object sender, RoutedEventArgs e)
        {
            IsAnImportantTransactionInstance2 = true;

        }

        private void chkInstance2_Unchecked(object sender, RoutedEventArgs e)
        {
            IsAnImportantTransactionInstance2 = false;
        }

        private void chkInstance1_Unchecked(object sender, RoutedEventArgs e)
        {
            IsAnImportantTransactionInstance1 = false;
        }
    }
}
