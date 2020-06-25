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
#include "Scenario3_Region.h"
#include "Scenario3_Region.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::UI::Xaml;

namespace
{
    hstring ReportRegionData(GeographicRegion const& region)
    {
        return L"Display Name: " + region.DisplayName() + L"\n" +
            L"Native Name: " + region.NativeName() + L"\n" +
            L"Currencies in use: " + SDKTemplate::StringJoin(L", ", region.CurrenciesInUse()) + L"\n" +
            L"Codes: " + region.CodeTwoLetter() + L", " + region.CodeThreeLetter() + L", " + region.CodeThreeDigit() + L"\n\n";
    }

}
namespace winrt::SDKTemplate::implementation
{
    Scenario3_Region::Scenario3_Region()
    {
        InitializeComponent();
    }

    void Scenario3_Region::ShowResults_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // This scenario uses the Windows.Globalization.GeographicRegion class to
        // obtain the geographic region characteristics.
        GeographicRegion userGeoRegion;

        // This obtains the geographic region characteristics by providing a country or region code.
        GeographicRegion exampleGeoRegion(L"JP");

        // Display the results
        OutputTextBlock().Text(L"User's Preferred Geographic Region\n" + ReportRegionData(userGeoRegion) +
            L"Example Geographic Region by region/country code (JP)\n" + ReportRegionData(exampleGeoRegion));
    }
}
