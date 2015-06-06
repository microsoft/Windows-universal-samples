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

using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using System.Threading.Tasks;
using SDKTemplate;
using Windows.Security.EnterpriseData;



namespace EdpSample
{

    public sealed partial class Scenario11_CpToClipboard : Page
    {
        private MainPage rootPage;

        public Scenario11_CpToClipboard()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            InputTxtBox.Text = "Text here will be copied to clipboard";
        }

        private void CopyToClipboard_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var dataPackage = new Windows.ApplicationModel.DataTransfer.DataPackage();
                dataPackage.SetText(InputTxtBox.Text);
                dataPackage.Properties.EnterpriseId = Scenario1.m_EnterpriseIdentity;
                Windows.ApplicationModel.DataTransfer.Clipboard.SetContent(dataPackage);
                rootPage.NotifyUser("Successfully SetContent to clipboard", NotifyType.StatusMessage);
            }
            catch(Exception ex)
            {
                rootPage.NotifyUser("Exception: " + ex.ToString(), NotifyType.ErrorMessage);
            }
               
        }


    }
}
