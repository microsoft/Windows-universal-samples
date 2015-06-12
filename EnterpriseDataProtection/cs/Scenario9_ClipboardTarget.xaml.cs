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
    public sealed partial class Scenario9_ClipboardTarget : Page
    {
        private MainPage rootPage;

        public Scenario9_ClipboardTarget()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            var protectionPolicyManager = ProtectionPolicyManager.GetForCurrentView();
            protectionPolicyManager.Identity = ""; // personal context
            InputTxtBox.PlaceholderText = "Copy Text here from a non-enterprise app";
        }

        private async void PasteButton_Click(object sender, TextControlPasteEventArgs e)
        {
            // Mark the event as handled first. Otherwise, the
            // default paste action will happen, then the custom paste
            // action, and the user will see the text box content change.
            e.Handled = true;

            // Get content from the clipboard.
            var dataPackageView = Windows.ApplicationModel.DataTransfer.Clipboard.GetContent();
            if (dataPackageView.Contains(Windows.ApplicationModel.DataTransfer.StandardDataFormats.Text))
            {
                var response = await dataPackageView.RequestAccessAsync();
                if (response == ProtectionPolicyEvaluationResult.Allowed)
                {
                    (sender as TextBox).Text = await dataPackageView.GetTextAsync();
                    string sourceId = dataPackageView.Properties.EnterpriseId != null ? dataPackageView.Properties.EnterpriseId : "<Personal>";
                    string targetId = ProtectionPolicyManager.GetForCurrentView().Identity != null ? dataPackageView.Properties.EnterpriseId : "<Personal>";
                    string message = "Successfully pasted text from EnterpriseId " + sourceId + " to EnterpriseId " + targetId;
                    rootPage.NotifyUser(message, NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("User or policy blocked access", NotifyType.StatusMessage);
                }
            }
        }
    }
}
