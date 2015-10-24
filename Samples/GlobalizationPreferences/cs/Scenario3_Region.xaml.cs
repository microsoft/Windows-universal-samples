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

using Windows.Foundation;
using Windows.Globalization;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario3_Region : Page
    {
        public Scenario3_Region()
        {
            this.InitializeComponent();
        }

        private string ReportRegionData(GeographicRegion region)
        {
            return "Display Name: " + region.DisplayName + "\n" +
                "Native Name: " + region.NativeName + "\n" +
                "Currencies in use: " + string.Join(", ", region.CurrenciesInUse) + "\n" +
                "Codes: " + region.CodeTwoLetter + ", " + region.CodeThreeLetter + ", " + region.CodeThreeDigit + "\n\n";
        }

        private void ShowResults()
        {
            // This scenario uses the Windows.Globalization.GeographicRegion class to
            // obtain the geographic region characteristics.
            var userGeoRegion = new GeographicRegion();

            // This obtains the geographic region characteristics by providing a country or region code.
            var exampleGeoRegion = new GeographicRegion("JP");

            // Display the results
            OutputTextBlock.Text = "User's Preferred Geographic Region\n" + ReportRegionData(userGeoRegion) +
                "Example Geographic Region by region/country code (JP)\n" + ReportRegionData(exampleGeoRegion);
        }
    }
}
