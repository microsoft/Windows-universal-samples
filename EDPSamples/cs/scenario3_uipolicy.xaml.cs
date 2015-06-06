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
    public sealed partial class Scenario3 : Page
    {
        private MainPage rootPage;
        string m_copyPasteScenarioText = "An enlightened application is currently showing an enterprise content amongst" +
                                       "a set of contents that include both enterprise and personal data belonging to" +
                                       "the user.To ensure that there are no data leaks from the enterprise data" +
                                       "the app calls TryApplyProcessUIPolicy to make sure the OS knows about the current context of the app." +
                                       "The API returns false if the identity is not being managed by an enterprise policy." +
                                       "Copying and then pasting of the below data to a non-enterprise app should be blocked after Pressing the TryApplyProcessUIPolicy button";
        string m_clearPolicyText = " After pressing the ClearProcessUIPolicy button, copy+paste of the above data to a non-enterprise app should succeed";
        string m_testDataToCopy = "copy paste this to non-enterprise app";


        public Scenario3()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            CopyPasteScenarioText.Text = m_copyPasteScenarioText;
            CopyTestData.Text = m_testDataToCopy;
            ClearUIPolicyLabel.Text = m_clearPolicyText;
        }

        private void TryApplyProcessUIPolicy_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                bool result = ProtectionPolicyManager.TryApplyProcessUIPolicy(Scenario1.m_EnterpriseIdentity);
                rootPage.NotifyUser("TryApplyProcessUIPolicy returned " + result, NotifyType.ErrorMessage);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Exception thrown:" + ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        private void ClearProcessUIPolicy_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                ProtectionPolicyManager.ClearProcessUIPolicy();
                rootPage.NotifyUser("Copy+Paste from this app should now succeed", NotifyType.ErrorMessage);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Exception thrown:" + ex.ToString(), NotifyType.ErrorMessage);
            }
            
        }
    }
}
