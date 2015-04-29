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
   
    public sealed partial class Scenario12_CPDNoUI : Page
    {
        private MainPage rootPage;


        public Scenario12_CPDNoUI()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            try
            {
                // Set view as personal

                ProtectionPolicyManager protectionPolicyManager = ProtectionPolicyManager.GetForCurrentView();
                protectionPolicyManager.Identity = "";
                InputTxtBox.PlaceholderText = "Paste any text here";
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }

        }

        private async void Set_ClipboardNoUI(object sender, TextControlPasteEventArgs e)
        {
            TextBox txtBox = sender as TextBox;
            string outputStr = "";

            try
            {

                if (txtBox != null)
               {
                // Mark the event as handled first. Otherwise, the
                // default paste action will happen, then the custom paste
                // action, and the user will see the text box content change.
                e.Handled = true;

                // Get content from the clipboard.
                var dataPackageView = Windows.ApplicationModel.DataTransfer.Clipboard.GetContent();
                    if (dataPackageView.Contains(Windows.ApplicationModel.DataTransfer.StandardDataFormats.Text))
                    {
                      //  ProtectionPolicyResult policyResult = await dataPackageView.UnlockAndAssumeIdentity();
                       // if (policyResult != ProtectionPolicyResult.Allowed)
                      //  {
                       //     outputStr += "Can access clipboard";
                       // }
                       // else
                      //  {
                            //await dataPackageView.RequestAccessAsync();
                       // }
                        var text = await dataPackageView.GetTextAsync();
                        txtBox.Text = text;
                        if(text != null)
                        {
                            outputStr = "Successfully pasted text";
                        }else
                        {
                          outputStr = "Pasting text failed";
                        }

                        rootPage.NotifyUser(outputStr, NotifyType.StatusMessage);
                    
                  }
               }
            }
             catch (Exception ex)
            {
                rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }

        }

    }
}
