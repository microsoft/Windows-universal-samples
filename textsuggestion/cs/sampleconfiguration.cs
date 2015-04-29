// Copyright (c) Microsoft Corporation. All rights reserved

using System;
using System.Collections.Generic;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "TextSuggestion";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Conversion", ClassType=typeof(Scenario1_Conversion)},
            new Scenario() { Title="Prediction", ClassType=typeof(Scenario2_Prediction)},
            new Scenario() { Title="ReverseConversion", ClassType=typeof(Scenario3_ReverseConversion)}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
