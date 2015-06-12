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
using Windows.ApplicationModel.DataTransfer;
using Windows.Foundation;

namespace EdpSample
{
   
    public sealed partial class Scenario12_ShareData : Page
    {
        private MainPage rootPage;
        private DataTransferManager m_dTransferMgr;
        private string m_ShareContent = "Content to share from EdpSamples";

        public Scenario12_ShareData()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            // Register the current page as a share source. 
            m_dTransferMgr = DataTransferManager.GetForCurrentView();
            m_dTransferMgr.DataRequested += new TypedEventHandler<DataTransferManager, DataRequestedEventArgs>(this.OnDataRequested);
            InputTxtBox.Text = m_ShareContent;
        }

        private void OnDataRequested(DataTransferManager sender, DataRequestedEventArgs e)
        {
            DataRequest request = e.Request;
            request.Data.Properties.Title = "EdpSample share";
            request.Data.Properties.Description = "An example of how to share text from an enterprise app";
            request.Data.Properties.EnterpriseId = Scenario1.m_enterpriseId;
            request.Data.SetText(InputTxtBox.Text);
            rootPage.NotifyUser("Set data to share", NotifyType.StatusMessage);
        }

        private void Share_Click(object sender, RoutedEventArgs e)
        {
            DataTransferManager.ShowShareUI();
        }
    }
}
