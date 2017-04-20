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
using Windows.UI.Xaml.Controls;
using PosPrinterSample;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "POS Printer";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Receipt Printer", ClassType=typeof(Scenario1_ReceiptPrinter)},
            new Scenario() { Title="Receipt Printer Error Handling", ClassType=typeof(Scenario2_ErrorHandling)},
            new Scenario() { Title="Multiple Receipt Printers", ClassType=typeof(Scenario3_MultipleReceipt)}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }

    public partial class DeviceHelpers
    {
        // By default, use all connections types.
        public static async Task<PosPrinter> GetFirstReceiptPrinterAsync(PosConnectionTypes connectionTypes = PosConnectionTypes.All)
        {
            return await DeviceHelpers.GetFirstDeviceAsync(PosPrinter.GetDeviceSelector(connectionTypes),
                async (id) =>
                {
                    PosPrinter printer = await PosPrinter.FromIdAsync(id);
                    if (printer != null && printer.Capabilities.Receipt.IsPrinterPresent)
                    {
                        return printer;
                    }
                    // Dispose the unwanted printer.
                    printer?.Dispose();
                    return null;
                });
        }
    }
}
