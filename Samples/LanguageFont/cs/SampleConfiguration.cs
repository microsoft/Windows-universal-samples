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
using Windows.Globalization.Fonts;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Language Font CS Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Fonts for UI", ClassType = typeof(Scenario1_UIFonts) },
            new Scenario() { Title = "Fonts for Documents", ClassType = typeof(Scenario2_DocumentFonts) }
        };

        public static void ApplyLanguageFont(TextBlock textBlock, LanguageFont languageFont)
        {
            FontFamily fontFamily = new FontFamily(languageFont.FontFamily);
            textBlock.FontFamily = fontFamily;
            textBlock.FontWeight = languageFont.FontWeight;
            textBlock.FontStyle = languageFont.FontStyle;
            textBlock.FontStretch = languageFont.FontStretch;
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
