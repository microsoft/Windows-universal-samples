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

    public sealed partial class Scenario11_CPDNoUI : Page
    {
        private MainPage rootPage;

        public Scenario11_CPDNoUI()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            var protectionPolicyManager = ProtectionPolicyManager.GetForCurrentView();
            protectionPolicyManager.Identity = ""; // personal
            InputTxtBox.PlaceholderText = "Paste any text here";
        }

        private async void Set_ClipboardNoUI(object sender, TextControlPasteEventArgs e)
        {
            // Mark the event as handled first. Otherwise, the
            // default paste action will happen, then the custom paste
            // action, and the user will see the text box content change.
            e.Handled = true;

            // Get content from the clipboard.
            var dataPackageView = Windows.ApplicationModel.DataTransfer.Clipboard.GetContent();
            if (dataPackageView.Contains(Windows.ApplicationModel.DataTransfer.StandardDataFormats.Text))
            {
                string outputStr = "";

                var policyResult = dataPackageView.UnlockAndAssumeEnterpriseIdentity();
                if (policyResult != ProtectionPolicyEvaluationResult.Allowed)
                {
                    outputStr += "Policy does not allow accessing clipboard";
                }
                var text = await dataPackageView.GetTextAsync();
                var txtBox = sender as TextBox;
                txtBox.Text = text;
                outputStr += text != null ? "Successfully pasted" : "Pasting text failed";
                rootPage.NotifyUser(outputStr, NotifyType.StatusMessage);
            }
        }
    }
}
