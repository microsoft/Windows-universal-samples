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


// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace EdpSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario15_QueryCpPaste : Page
    {
        private MainPage rootPage;
 
        public Scenario15_QueryCpPaste()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        bool IsClipboardPeekAllowedAsync()

        {
            ProtectionPolicyEvaluationResult result = ProtectionPolicyEvaluationResult.Blocked;
            var dataPackageView = Clipboard.GetContent();
            if (dataPackageView.Contains("text"))
            {
                var protectionPolicyManager = ProtectionPolicyManager.GetForCurrentView();
                result = ProtectionPolicyManager.CheckAccess(
                            dataPackageView.Properties.EnterpriseId,
                            protectionPolicyManager.Identity);
            }

            // Since this is a convenience feature to allow a peek of clipboard content, 
            // if state is Blocked or ConsentRequired, do not show peek. 

            return (result == ProtectionPolicyEvaluationResult.Allowed);

        }


        private void QueryCp_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                rootPage.NotifyUser("Check Access for Paste returned: " + IsClipboardPeekAllowedAsync(), NotifyType.StatusMessage);
            }catch(Exception ex)
            {
                rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }
        }

    }

}
