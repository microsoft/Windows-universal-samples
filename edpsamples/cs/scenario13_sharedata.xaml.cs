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
    
    public sealed partial class Scenario13_ShareData : Page
    {
        private MainPage rootPage;
        private DataTransferManager m_dTransferMgr;
        private string m_ShareContent = "Content to share from EdpSamples";


        public Scenario13_ShareData()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            try
            {
                // Register the current page as a share source. 
                m_dTransferMgr = DataTransferManager.GetForCurrentView();
                m_dTransferMgr.DataRequested += new TypedEventHandler<DataTransferManager, DataRequestedEventArgs>(this.OnDataRequested);
                InputTxtBox.Text = m_ShareContent;
            }
            catch(Exception ex)
            {
                rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        private void OnDataRequested(DataTransferManager sender, DataRequestedEventArgs e)

        {
            try
            {
                DataRequest request = e.Request;
                request.Data.Properties.Title = "EdpSample share";
                request.Data.Properties.Description = "An example of how to share text from an enterprise app";
                request.Data.Properties.EnterpriseId = Scenario1.m_EnterpriseIdentity;
                request.Data.SetText(InputTxtBox.Text);
                rootPage.NotifyUser("Set data to share", NotifyType.StatusMessage);
            }
            catch(Exception ex)
            {
                rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        private void Share_Click(object sender, RoutedEventArgs e)
        {
            DataTransferManager.ShowShareUI();
        }

    }

}
