//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using Windows.ApplicationModel.Resources.Core;
using System.Collections.Generic;
using System.Text;
using Windows.Globalization;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario8 : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario8()
        {
            this.InitializeComponent();
            UpdateCurrentAppLanguageMessage();
            LanguageOverrideCombo.LanguageOverrideChanged += LanguageOverrideCombo_LanguageOrverrideChanged;
        }

        void LanguageOverrideCombo_LanguageOrverrideChanged(object sender, EventArgs e)
        {
            UpdateCurrentAppLanguageMessage();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {

        }

        /// <summary>
        /// This is the click handler for the 'Scenario8Button_Show' button.  You would replace this with your own handler
        /// if you have a button or buttons on this page.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Scenario8Button_Show_Click(object sender, RoutedEventArgs e)
        {
            Button b = sender as Button;
            if (b != null)
            {
                ResourceContext defaultContextForCurrentView = ResourceContext.GetForCurrentView();
                ResourceMap stringResourcesResourceMap = ResourceManager.Current.MainResourceMap.GetSubtree("Resources");
                Scenario8MessageTextBlock.Text = stringResourcesResourceMap.GetValue("string1", defaultContextForCurrentView).ValueAsString;
            }
        }


        private void UpdateCurrentAppLanguageMessage()
        {
            this.Scenario8AppLanguagesTextBlock.Text = "Current app language(s): " + GetAppLanguagesAsFormattedString();
        }

        private string GetAppLanguagesAsFormattedString()
        {
            var countLanguages = ApplicationLanguages.Languages.Count;
            StringBuilder sb = new StringBuilder();
            for (var i = 0; i < countLanguages - 1; i++)
            {
                sb.Append(ApplicationLanguages.Languages[i]);
                sb.Append(", ");
            }
            sb.Append(ApplicationLanguages.Languages[countLanguages - 1]);
            return sb.ToString();
        }
    }
}
