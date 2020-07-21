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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Sample;

namespace SDKTemplate
{
    public sealed partial class Scenario3_Transliteration : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario3_Transliteration()
        {
            InitializeComponent();
        }

        public void Go_Click(object sender, RoutedEventArgs e)
        {
            TextOutput.Text = LinguisticServices.TransliterateFromCyrillicToLatin(TextInput.Text);
        }
    }
}
