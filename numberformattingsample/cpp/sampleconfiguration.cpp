// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Percent and Permille Formatting", "NumberFormatting.Scenario1_PercentPermilleFormatting" }, 
    { "Decimal Formatting", "NumberFormatting.Scenario2_DecimalFormatting" },
    { "Currency Formatting", "NumberFormatting.Scenario3_CurrencyFormatting" },
    { "Number Parsing", "NumberFormatting.Scenario4_NumberParsing" },
    { "Rounding and Padding", "NumberFormatting.Scenario5_RoundingAndPadding" },
    { "Numeral System Translation", "NumberFormatting.Scenario6_NumeralSystemTranslation" },
    { "Formatting/Translation using Unicode Extensions", "NumberFormatting.Scenario7_UsingUnicodeExtensions" }
};
