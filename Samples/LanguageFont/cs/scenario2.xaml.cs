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
    public sealed partial class Scenario2 : Page
    {
        private LocalFontInfo oriHeadingDoc;
        private LocalFontInfo oriTextDoc;

        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario2()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (this.oriHeadingDoc != null)
            {
                var headingDoc = (Windows.UI.Xaml.Controls.TextBlock)this.FindName("Scenario2Heading");
                oriHeadingDoc.Reset(headingDoc);

                var textDoc = (Windows.UI.Xaml.Controls.TextBlock)this.FindName("Scenario2Text");
                oriTextDoc.Reset(textDoc);
            }
        }

        /// <summary>
        /// This is the click handler for the 'Scenario2InputButton' button.  You would replace this with your own handler
        /// if you have a button or buttons on this page.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Scenario2InputButton_Click(object sender, RoutedEventArgs e)
        {
            Button b = sender as Button;
            if (b != null)
            {
                var languageFontGroup = new LanguageFontGroup("hi");
                var headingUI = (Windows.UI.Xaml.Controls.TextBlock)this.Scenario2Heading;
                var textUI = (Windows.UI.Xaml.Controls.TextBlock)this.Scenario2Text;

                if (this.oriHeadingDoc == null)
                {
                    this.oriHeadingDoc = new LocalFontInfo();
                    this.oriTextDoc = new LocalFontInfo();
                    this.oriHeadingDoc.Set(headingUI);
                    this.oriTextDoc.Set(textUI);
                }

                this.SetFont(headingUI, languageFontGroup.DocumentHeadingFont);

                // Not all scripts have recommended fonts for "document alternate"
                // categories, so need to verify before using it. Note that Hindi does
                // have document alternate fonts, so in this case the fallback logic is
                // unnecessary, but (for example) Japanese does not have recommendations
                // for the document alternate 2 category.
                if (languageFontGroup.DocumentAlternate2Font != null)
                {
                    this.SetFont(textUI, languageFontGroup.DocumentAlternate2Font);
                }
                else if (languageFontGroup.DocumentAlternate1Font != null)
                {
                    this.SetFont(textUI, languageFontGroup.DocumentAlternate1Font);
                }
                else
                {
                    this.SetFont(textUI, languageFontGroup.ModernDocumentFont);
                }

                this.Output.Visibility = Visibility.Visible;
            }
        }

        private void SetFont(TextBlock textBlock, Windows.Globalization.Fonts.LanguageFont languageFont)
        {
            FontFamily fontFamily = new FontFamily(languageFont.FontFamily);
            textBlock.FontFamily = fontFamily;
            textBlock.FontWeight = languageFont.FontWeight;
            textBlock.FontStyle = languageFont.FontStyle;
            textBlock.FontStretch = languageFont.FontStretch;
        }
    }
}
