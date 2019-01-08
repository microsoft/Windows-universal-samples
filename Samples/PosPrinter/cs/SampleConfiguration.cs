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
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Devices.PointOfService;
using Windows.UI.Core;
using Windows.UI.Xaml.Controls;
using Windows.Devices.Enumeration;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "POS Printer C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Find, claim, and enable printer", ClassType=typeof(Scenario1_FindClaimEnable)},
            new Scenario() { Title="Print receipt", ClassType=typeof(Scenario2_PrintReceipt)},
            new Scenario() { Title="Multiple claims", ClassType=typeof(Scenario3_MultipleClaims)},
        };

        internal PosPrinter Printer = null;
        internal ClaimedPosPrinter ClaimedPrinter = null;
        internal DeviceInformation deviceInfo = null;
        internal bool IsAnImportantTransaction = true;
        internal event Action StateChanged;

        public void SubscribeToReleaseDeviceRequested()
        {
            ClaimedPrinter.ReleaseDeviceRequested += ClaimedPrinter_ReleaseDeviceRequested;
        }

        public void ReleaseClaimedPrinter()
        {
            if (ClaimedPrinter != null)
            {
                ClaimedPrinter.ReleaseDeviceRequested -= ClaimedPrinter_ReleaseDeviceRequested;
                ClaimedPrinter.Dispose();
                ClaimedPrinter = null;
                StateChanged?.Invoke();
            }
        }

        public void ReleaseAllPrinters()
        {
            ReleaseClaimedPrinter();

            if (Printer != null)
            {
                Printer.Dispose();
                Printer = null;
                StateChanged?.Invoke();
            }
        }

        /// <summary>
        /// If the "Retain device" checkbox is checked, we retain the device.
        /// Otherwise, we allow the other claimant to claim the device.
        /// </summary>
        private async void ClaimedPrinter_ReleaseDeviceRequested(ClaimedPosPrinter sender, PosPrinterReleaseDeviceRequestedEventArgs args)
        {
            if (IsAnImportantTransaction)
            {
                await sender.RetainDeviceAsync();
            }
            else
            {
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    NotifyUser("Lost printer claim.", NotifyType.ErrorMessage);
                    ReleaseClaimedPrinter();
                });
            }
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
