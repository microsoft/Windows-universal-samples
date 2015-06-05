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
using SDKTemplate;
using Windows.Security.EnterpriseData;



namespace EdpSample
{
   
    public sealed partial class Scenario9_ClipboardSource : Page
    {
        private MainPage rootPage;
     
        public Scenario9_ClipboardSource()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            InputTxtBox.Text = "Copy and paste this text to a non-enterprise app such as notepad";
        }

        private void Set_ClipboardSource(object sender, RoutedEventArgs e)
        {
            try
            {
                ProtectionPolicyManager ppManager = ProtectionPolicyManager.GetForCurrentView();
                ppManager.Identity = Scenario1.m_EnterpriseIdentity;
                rootPage.NotifyUser("Set ProtectionPolicyManager.Identity to: " + Scenario1.m_EnterpriseIdentity, NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }
        }

    }
}
