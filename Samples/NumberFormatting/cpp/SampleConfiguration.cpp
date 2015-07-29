// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Percent and Permille Formatting", "SDKTemplate.Scenario1_PercentPermilleFormatting" }, 
    { "Decimal Formatting", "SDKTemplate.Scenario2_DecimalFormatting" },
    { "Currency Formatting", "SDKTemplate.Scenario3_CurrencyFormatting" },
    { "Number Parsing", "SDKTemplate.Scenario4_NumberParsing" },
    { "Rounding and Padding", "SDKTemplate.Scenario5_RoundingAndPadding" },
    { "Numeral System Translation", "SDKTemplate.Scenario6_NumeralSystemTranslation" },
    { "Formatting/Translation using Unicode Extensions", "SDKTemplate.Scenario7_UsingUnicodeExtensions" }
};
