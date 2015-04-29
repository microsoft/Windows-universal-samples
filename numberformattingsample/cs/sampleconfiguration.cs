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
using Windows.UI.Xaml.Controls;
using NumberFormatting;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Number formatting C# sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Percent and Permille Formatting", ClassType = typeof(Scenario1_PercentPermilleFormatting) },
            new Scenario() { Title = "Decimal Formatting", ClassType = typeof(Scenario2_DecimalFormatting) },
            new Scenario() { Title = "Currency Formatting", ClassType = typeof(Scenario3_CurrencyFormatting) },
            new Scenario() { Title = "Number Parsing", ClassType = typeof(Scenario4_NumberParsing) },
            new Scenario() { Title = "Rounding and Padding", ClassType = typeof(Scenario5_RoundingAndPadding) },
            new Scenario() { Title = "Numeral System Translation", ClassType = typeof(Scenario6_NumeralSystemTranslation) },
            new Scenario() { Title = "Formatting/Translation using Unicode Extensions", ClassType = typeof(Scenario7_UsingUnicodeExtensions) }
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
