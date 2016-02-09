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
#include "pch.h"
#include "Scenario3_Region.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;

Scenario3_Region::Scenario3_Region()
{
    InitializeComponent();
}

String^ Scenario3_Region::ReportRegionData(GeographicRegion^ region)
{
    return "Display Name: " + region->DisplayName + "\n" +
        "Native Name: " + region->NativeName + "\n" +
        "Currencies in use: " + StringJoin(", ", region->CurrenciesInUse) + "\n" +
        "Codes: " + region->CodeTwoLetter + ", " + region->CodeThreeLetter + ", " + region->CodeThreeDigit + "\n\n";
}

void Scenario3_Region::ShowResults()
{
    // This scenario uses the Windows.Globalization.GeographicRegion class to
    // obtain the geographic region characteristics.
    auto userGeoRegion = ref new GeographicRegion();

    // This obtains the geographic region characteristics by providing a country or region code.
    auto exampleGeoRegion = ref new GeographicRegion("JP");

    // Display the results
    OutputTextBlock->Text = "User's Preferred Geographic Region\n" + ReportRegionData(userGeoRegion) +
        "Example Geographic Region by region/country code (JP)\n" + ReportRegionData(exampleGeoRegion);
}
