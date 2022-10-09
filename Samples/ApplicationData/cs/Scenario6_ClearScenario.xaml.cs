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
using System;

namespace SDKTemplate
{
    public sealed partial class Scenario6_ClearScenario : Page
    {
        public Scenario6_ClearScenario()
        {
            InitializeComponent();
        }

        async void Clear_Click(Object sender, RoutedEventArgs e)
        {
            try
            {
                await Windows.Storage.ApplicationData.Current.ClearAsync();
                OutputTextBlock.Text = "ApplicationData has been cleared.  Visit the other scenarios to see that their data has been cleared.";
            }
            catch (Exception)
            {
                OutputTextBlock.Text = "Unable to clear settings. This can happen when files are in use.";
            }
        }
    }
}
