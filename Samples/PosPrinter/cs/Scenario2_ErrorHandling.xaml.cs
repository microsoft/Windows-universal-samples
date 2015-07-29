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
    public sealed partial class Scenario2_ErrorHandling : Page
    {
        private MainPage rootPage;

        PosPrinter printer = null;
        ClaimedPosPrinter claimedPrinter = null;
        bool IsAnImportantTransaction = true;

        public Scenario2_ErrorHandling()
        {
            this.InitializeComponent();
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

        private void ResetTheScenarioState()
        {
            //Remove releasedevicerequested handler and dispose claimed printer object.
            if (claimedPrinter != null)
            {
                claimedPrinter.ReleaseDeviceRequested -= ClaimedPrinter_ReleaseDeviceRequested;
                claimedPrinter.Dispose();
                claimedPrinter = null;
            }

            printer = null;
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

        async void PrintLine_Click(object sender, RoutedEventArgs e)
        {
            await PrintLineAndCheckForErrors();
        }

        /// <summary>
        /// Prints the line that is in the textbox. Then checks for any error conditions and reports the same to user.
        /// </summary>
        private async Task<bool> PrintLineAndCheckForErrors()
        {
            if (!IsPrinterClaimed())
            {
                return false;
            }
            ReceiptPrintJob job = claimedPrinter.Receipt.CreateJob();
            job.PrintLine(txtPrintLine.Text);

            if (await job.ExecuteAsync())
            {
                rootPage.NotifyUser("Printed line", NotifyType.StatusMessage);
                return true;
            }
            else
            {
                if (claimedPrinter.Receipt.IsCartridgeEmpty)
                {
                    rootPage.NotifyUser("Printer is out of ink. Please replace cartridge.", NotifyType.StatusMessage);
                }
                else if (claimedPrinter.Receipt.IsCartridgeRemoved)
                {
                    rootPage.NotifyUser("Printer cartridge is missing. Please replace cartridge.", NotifyType.StatusMessage);
                }
                else if (claimedPrinter.Receipt.IsCoverOpen)
                {
                    rootPage.NotifyUser("Printer cover is open. Please close it.", NotifyType.StatusMessage);
                }
                else if (claimedPrinter.Receipt.IsHeadCleaning)
                {
                    rootPage.NotifyUser("Printer is currently cleaning the cartridge. Please wait until cleaning has completed.", NotifyType.StatusMessage);
                }
                else if (claimedPrinter.Receipt.IsPaperEmpty)
                {
                    rootPage.NotifyUser("Printer is out of paper. Please insert a new roll.", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("Was not able to print line", NotifyType.StatusMessage);
                }
            }
            return false;
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
                claimedPrinter.ReleaseDeviceRequested += ClaimedPrinter_ReleaseDeviceRequested;
                rootPage.NotifyUser("No Claimed Printer to enable", NotifyType.ErrorMessage);
                return false;
            }

            if (!await claimedPrinter.EnableAsync())
            {
                rootPage.NotifyUser("Could not enable printer", NotifyType.ErrorMessage);
                return false;
            }

            rootPage.NotifyUser("Enabled Printer", NotifyType.StatusMessage);
            return true;
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

        /// <summary>
        /// GetDeviceSelector method returns the string needed to identify a PosPrinter. This is passed to FindAllAsync method to get the list of devices currently available and we connect the first device.
        /// </summary>
        private async Task<bool> FindReceiptPrinter()
        {
            if (printer == null)
            {
                rootPage.NotifyUser("Finding printer", NotifyType.StatusMessage);
                DeviceInformationCollection deviceCollection = await DeviceInformation.FindAllAsync(PosPrinter.GetDeviceSelector());
                if (deviceCollection != null && deviceCollection.Count > 0)
                {
                    DeviceInformation deviceInfo = deviceCollection[0];
                    printer = await PosPrinter.FromIdAsync(deviceInfo.Id);
                    if (printer != null)
                    {
                        if (printer.Capabilities.Receipt.IsPrinterPresent)
                        {
                            rootPage.NotifyUser("Got Printer with Device Id : " + printer.DeviceId, NotifyType.StatusMessage);
                            return true;
                        }
                    }
                    else
                    {
                        rootPage.NotifyUser("No Printer found", NotifyType.ErrorMessage);
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
    }
}
