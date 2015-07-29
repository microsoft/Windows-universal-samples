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

using Windows.UI.Xaml.Controls;


namespace SDKTemplate
{
    public sealed partial class Scenario_Document2 : Page
    {
        public Scenario_Document2()
        {
            this.InitializeComponent();
        }

        private void Page_Loaded(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            // Font formatting is being applied on page load rather than in XAML markup
            // so that the on-demand cloud font behavior can be experienced when the
            // app runs rather than when the XAML file is viewed in Visual Studio.

            // Formatting will be applied to all content elements in the page that don't 
            // already have FontFamily set.
            this.FontFamily = new Windows.UI.Xaml.Media.FontFamily("FangSong");
        }
    }
}
