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

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace EdpSample
{

    public sealed partial class Scenario10_ClipboardTarget : Page
    {
        private MainPage rootPage;


        public Scenario10_ClipboardTarget()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            try
            {
                // Set protectionPolicyManager.Identity to "" so that current view is personal

                ProtectionPolicyManager protectionPolicyManager = ProtectionPolicyManager.GetForCurrentView();
                protectionPolicyManager.Identity = "";
                InputTxtBox.PlaceholderText = "Copy Text here from a non-enterprise app";
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }

        }

        private async void PasteButton_Click(object sender, TextControlPasteEventArgs e)
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
                   
                        //await dataPackageView.RequestAccessAsync();
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
