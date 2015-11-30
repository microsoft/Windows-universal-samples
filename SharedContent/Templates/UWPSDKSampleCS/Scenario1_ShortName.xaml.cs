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
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1_ShortName : Page
    {
        private MainPage rootPage;

        public Scenario1_ShortName()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("Hello", NotifyType.StatusMessage);
        }

        private void Button_Click_2(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("Hello", NotifyType.ErrorMessage);
        }

        private void Button_Click_3(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("Hello How are you today. This is a very long message with a lot of text.\nLet's throw in some newline characters\nand\nsee\nwhat\nhappens.\nadsf\nasdf\nasdf\nasfd\nasdf\nasdf\nasfd", NotifyType.StatusMessage);
        }
    }
}
