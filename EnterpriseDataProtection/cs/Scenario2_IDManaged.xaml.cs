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

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace EdpSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2 : Page
    {
        private MainPage rootPage;
        private string m_scenarioText = "An identity being used by an app might be provisioned to have " +
                                      "an enterprise policy enforced.When an enterprise policy is enforced on the identity, " +
                                      "the app must help the system with policy enforcement by calling APIs under ProtectionPolicyManager " +
                                      "during UI activities or network accesses to ensure that the system tags data transfers with this " +
                                      "identity when necessary.This scenario example shows how the app can check if an identity is managed " +
                                      "by an enterprise policy.";


        public Scenario2()
        {
            this.InitializeComponent();
        }


        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            ScenarioTextTbl.Text = m_scenarioText;
            rootPage = MainPage.Current;
        }

        private void IsIdentityManaged_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                bool isIdentityManaged = ProtectionPolicyManager.IsIdentityManaged(Scenario1.m_EnterpriseIdentity);
                rootPage.NotifyUser("IsIdentityManaged: " + isIdentityManaged, NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Exception thrown:" + ex.ToString(), NotifyType.ErrorMessage);
            }
        }
    }
}
