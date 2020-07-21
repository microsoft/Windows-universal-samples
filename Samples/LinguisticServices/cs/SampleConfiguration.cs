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

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Linguistic services C# sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Language Detection", ClassType = typeof(Scenario1_LanguageDetection) },
            new Scenario() { Title = "Script Detection", ClassType = typeof(Scenario2_ScriptDetection) },
            new Scenario() { Title = "Transliteration", ClassType = typeof(Scenario3_Transliteration) },
            new Scenario() { Title = "Character Grouping", ClassType = typeof(Scenario4_CharacterGrouping) },
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
