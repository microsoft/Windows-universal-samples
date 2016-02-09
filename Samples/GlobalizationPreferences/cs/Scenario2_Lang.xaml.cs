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

using Windows.Foundation;
using Windows.Globalization;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario2_Lang : Page
    {
        public Scenario2_Lang()
        {
            this.InitializeComponent();
        }

        private string ReportLanguageData(Language lang)
        {
            return "Display Name: " + lang.DisplayName + "\n" +
                "Language Tag: " + lang.LanguageTag + "\n" +
                "Native Name: " + lang.NativeName + "\n" +
                "Script Code: " + lang.Script + "\n\n";
        }

        private void ShowResults()
        {
            // This scenario uses the Windows.System.UserProfile.GlobalizationPreferences class to
            // obtain the user's preferred language characteristics.
            var topUserLanguage = Windows.System.UserProfile.GlobalizationPreferences.Languages[0];
            var userLanguage = new Language(topUserLanguage);

            // This obtains the language characteristics by providing a BCP47 tag.
            var exampleLanguage = new Language("ja");

            // Display the results
            OutputTextBlock.Text = "User's Preferred Language\n" + ReportLanguageData(userLanguage) +
                "Example Language by BCP47 tag (ja)\n" + ReportLanguageData(exampleLanguage);
        }
    }
}
