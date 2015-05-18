//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Security.EnterpriseData;
using SDKTemplate;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace EdpSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario6 : Page
    {
        private MainPage rootPage;
        private string m_revokeScenarioTxt = "A user has several enterprise files on their personal device which are protected " +
                                           "to the enterprise identity. The user loses their device. When notified, the enterprise " +
                                           "has sent a request to wipe all Enterprise data from the user’s device to prevent it from " +
                                           "leaking. The remote management client on the device has received this request from the enterprise’s " +
                                           "remote management server. The remote management client calls RevokeContent to revoke keys required to access " +
                                           "content protected to the enterprise identity.";
        private string m_revokeEventScenarioText = "\n\nIn order to be notified when a revoke happens, the app registers with the ProtectedContentRevoked event. " +
                                                   "When it receives the event, it deletes any metadata associated with the enterprise mailbox which will no longer be required. ";


        public Scenario6()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            RevokeScenarioText.Text = m_revokeScenarioTxt + m_revokeEventScenarioText;

            rootPage = MainPage.Current;
            try
            {
                ProtectionPolicyManager.ProtectedContentRevoked += ProtectedContentRevoked;
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        private void Revoke_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                ProtectionPolicyManager.RevokeContent(Scenario1.m_EnterpriseIdentity);
                rootPage.NotifyUser("RevokeContent executed", NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Exception thrown:" + ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        void ProtectedContentRevoked(Object sender, ProtectedContentRevokedEventArgs args)
        {
            RevokeEventHdlr.Text = "Content is Revoked event has fired. Do some housekeeping";
        }


    }

}
