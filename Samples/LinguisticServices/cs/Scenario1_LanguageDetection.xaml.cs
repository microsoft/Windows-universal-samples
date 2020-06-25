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
using System.Text;

namespace SDKTemplate
{
    public sealed partial class Scenario1_LanguageDetection : Page
    {
        public Scenario1_LanguageDetection()
        {
            InitializeComponent();
        }

        public void Go_Click(object sender, RoutedEventArgs e)
        {
            StringBuilder output = new StringBuilder();
            foreach (string language in LinguisticServices.RecognizeTextLanguages(TextInput.Text))
            {
                output.AppendLine(language);
            }
            TextOutput.Text = output.ToString();
        }
    }
}
