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
    public sealed partial class Scenario2_ScriptDetection : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario2_ScriptDetection()
        {
            InitializeComponent();
        }

        public void Go_Click(object sender, RoutedEventArgs e)
        {
            StringBuilder output = new StringBuilder();
            foreach (RecognizedTextRun run in LinguisticServices.RecognizeTextScripts(TextInput.Text))
            {
                output.AppendLine($"Range from {run.First} to {run.Last}: {run.Script} script");
            }
            TextOutput.Text = output.ToString();

        }
    }
}
