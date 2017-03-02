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

using SDKTemplate;
using System;
using System.Collections.Generic;
using System.Linq;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;

namespace SDKTemplate
{
    public sealed partial class Scenario1 : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario1()
        {
            this.InitializeComponent();
        }

        private void OpenMenuItem_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("Action: Open", NotifyType.StatusMessage);
        }

        private void PrintMenuItem_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("Action: Print", NotifyType.StatusMessage);
        }

        private void DeleteMenuItem_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("Action: Delete", NotifyType.StatusMessage);
        }
    }
}
