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
using Windows.Foundation;
using Windows.Globalization.Fonts;
using Windows.Graphics.Display;
using Windows.System;
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
    public sealed partial class Scenario1 : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        private LocalFontInfo oriHeadingUI;
        private LocalFontInfo oriTextUI;

        MainPage rootPage = MainPage.Current;

        public Scenario1()
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
            if (this.oriHeadingUI != null)
            {
                var headingUI = (Windows.UI.Xaml.Controls.TextBlock)this.Scenario1Heading;
                this.oriHeadingUI.Reset(headingUI);

                var textUI = (Windows.UI.Xaml.Controls.TextBlock)this.Scenario1Text;
                this.oriTextUI.Reset(textUI);
            }
        }


        /// <summary>
        /// This is the click handler for the 'Scenario1InputButton' button.  You would replace this with your own handler
        /// if you have a button or buttons on this page.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Scenario1InputButton_Click(object sender, RoutedEventArgs e)
        {
            Button b = sender as Button;
            if (b != null)
            {
                var languageFontGroup = new LanguageFontGroup("ja-JP");
                var headingUI = (Windows.UI.Xaml.Controls.TextBlock)this.Scenario1Heading;
                var textUI = (Windows.UI.Xaml.Controls.TextBlock)this.Scenario1Text;

                if (this.oriHeadingUI == null)
                {
                    // Store original font style for Reset
                    this.oriHeadingUI = new LocalFontInfo();
                    this.oriTextUI = new LocalFontInfo();
                    this.oriHeadingUI.Set(headingUI);
                    this.oriTextUI.Set(textUI);
                }

                // Change the Font value with selected font from LanguageFontGroup API
                this.SetFont(headingUI, languageFontGroup.UIHeadingFont);
                this.SetFont(textUI, languageFontGroup.UITextFont);

                this.Output.Visibility = Visibility.Visible;
            }
        }

        /// <summary>
        /// This is the click handler for the 'Other' button.  You would replace this with your own handler
        /// if you have a button or buttons on this page.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Other_Click(object sender, RoutedEventArgs e)
        {
            Button b = sender as Button;
            if (b != null)
            {
                rootPage.NotifyUser("You clicked the " + b.Name + " button", NotifyType.StatusMessage);
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
