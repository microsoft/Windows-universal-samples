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
    public sealed partial class Scenario8_ClipboardSource : Page
    {
        private MainPage rootPage;
     
        public Scenario8_ClipboardSource()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            var protectionPolicyManager = ProtectionPolicyManager.GetForCurrentView();
            protectionPolicyManager.Identity = ""; // personal context
            InputTxtBox.Text = "Copy and paste this text to a non-enterprise app such as notepad";
        }

        private void Set_ClipboardSource(object sender, RoutedEventArgs e)
        {
            var protectionPolicyManager = ProtectionPolicyManager.GetForCurrentView();
            protectionPolicyManager.Identity = Scenario1.m_enterpriseId;
            rootPage.NotifyUser("Set ProtectionPolicyManager.Identity to: " + Scenario1.m_enterpriseId, NotifyType.StatusMessage);
        }
    }
}
