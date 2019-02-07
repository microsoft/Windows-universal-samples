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

namespace SDKTemplate
{
    public sealed partial class Scenario1_ShortName : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario1_ShortName()
        {
            InitializeComponent();
        }

        private void StatusMessage_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("Hello", NotifyType.StatusMessage);
        }

        private void ErrorMessage_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("Oh dear.\nSomething went wrong.", NotifyType.ErrorMessage);
        }

        private void ClearMessage_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);
        }
    }
}
