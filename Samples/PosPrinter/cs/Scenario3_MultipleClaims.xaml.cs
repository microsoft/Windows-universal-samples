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
using Windows.Devices.PointOfService;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario3_MultipleClaims : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario3_MultipleClaims()
        {
            this.InitializeComponent();
        }

        async void ClaimSamePrinter_Click()
        {
            ClaimResultText.Text = "";
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            if (rootPage.Printer == null)
            {
                rootPage.NotifyUser("Use scenario 1 to find a printer first.", NotifyType.ErrorMessage);
                return;
            }

            using (var printer = await PosPrinter.FromIdAsync(rootPage.Printer.DeviceId))
            {
                if (printer == null)
                {
                    rootPage.NotifyUser("Cannot create same printer as scenario 1.", NotifyType.ErrorMessage);
                }

                using (var claimedPrinter = await printer.ClaimPrinterAsync())
                {
                    if (claimedPrinter != null)
                    {
                        ClaimResultText.Text = "Claimed the printer.";

                        // This scenario doesn't do anything with the printer aside from claim it.
                        // The "using" statement will dispose the claimed printer.
                    }
                    else
                    {
                        ClaimResultText.Text = "Did not claim the printer.";
                    }
                }
                // The "using" statement will dispose the duplicate printer.
            }
        }
    }
}
