//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using Windows.Globalization.Fonts;
using Windows.Graphics.Display;
using Windows.UI.Xaml;
using Windows.UI.Text;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Media;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2_DocumentFonts : Page
    {
        public Scenario2_DocumentFonts()
        {
            this.InitializeComponent();
        }

        private void ApplyFonts_Click(object sender, RoutedEventArgs e)
        {
            var languageFontGroup = new LanguageFontGroup("hi");

            MainPage.ApplyLanguageFont(HeadingTextBlock, languageFontGroup.DocumentHeadingFont);

            // Not all scripts have recommended fonts for "document alternate"
            // categories, so need to verify before using it. Note that Hindi does
            // have document alternate fonts, so in this case the fallback logic is
            // unnecessary, but (for example) Japanese does not have recommendations
            // for the document alternate 2 category.
            MainPage.ApplyLanguageFont(DocumentTextBlock,
                languageFontGroup.DocumentAlternate2Font ??
                languageFontGroup.DocumentAlternate1Font ??
                languageFontGroup.ModernDocumentFont);
        }
    }
}
