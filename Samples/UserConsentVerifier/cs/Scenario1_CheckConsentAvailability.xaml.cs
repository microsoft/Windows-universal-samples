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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Security.Credentials.UI;
using System;
using SDKTemplate;

namespace UserConsentVerifier
{
    public sealed partial class Scenario1_CheckConsentAvailability : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario1_CheckConsentAvailability()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// This is the click handler for the 'Check Availability' button. It checks the availability of Windows Hello via User Consent Verifier
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void CheckAvailability_Click(object sender, RoutedEventArgs e)
        {
            Button b = sender as Button;
            b.IsEnabled = false;
            try
            {
                // Check the availability of Windows Hello authentication through User Consent Verifier.
                UserConsentVerifierAvailability consentAvailability = await Windows.Security.Credentials.UI.UserConsentVerifier.CheckAvailabilityAsync();
                switch (consentAvailability)
                {
                    case UserConsentVerifierAvailability.Available:
                        {
                            rootPage.NotifyUser("User consent verification available!", NotifyType.StatusMessage);
                            break;
                        }

                    case UserConsentVerifierAvailability.DeviceNotPresent:
                        {
                            rootPage.NotifyUser("No PIN or biometric found, please set one up.", NotifyType.ErrorMessage);
                            break;
                        }

                    default:
                        {
                            rootPage.NotifyUser("User consent verification is currently unavailable.", NotifyType.ErrorMessage);
                            break;
                        }
                }
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Checking the availability of Consent feature failed with exception. Operation: CheckAvailabilityAsync, Exception: " + ex.ToString(), NotifyType.ErrorMessage);
            }
            finally
            {
                b.IsEnabled = true;
            }
        }
    }
}
