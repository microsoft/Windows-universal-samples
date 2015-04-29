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
    public sealed partial class Scenario2_RequestConsent : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario2_RequestConsent()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// This is the click handler for the 'Request Consent' button. It requests consent from the current user.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void RequestConsent_Click(object sender, RoutedEventArgs e)
        {
            Button b = sender as Button;
            b.IsEnabled = false;

            if (!String.IsNullOrEmpty(Message.Text))
            {
                try
                {
                    // Request the currently logged on user's consent via fingerprint swipe
                    UserConsentVerificationResult consentResult = await Windows.Security.Credentials.UI.UserConsentVerifier.RequestVerificationAsync(Message.Text);
                    switch (consentResult)
                    {
                        case UserConsentVerificationResult.Verified:
                            {
                                rootPage.NotifyUser("User's presence verified.", NotifyType.StatusMessage);
                                break;
                            }

                        case UserConsentVerificationResult.DeviceBusy:
                            {
                                rootPage.NotifyUser("Biometric device is busy.", NotifyType.ErrorMessage);
                                break;
                            }

                        case UserConsentVerificationResult.DeviceNotPresent:
                            {
                                rootPage.NotifyUser("No biometric device found.", NotifyType.ErrorMessage);
                                break;
                            }

                        case UserConsentVerificationResult.DisabledByPolicy:
                            {
                                rootPage.NotifyUser("Biometrics is disabled by policy.", NotifyType.ErrorMessage);
                                break;
                            }

                        case UserConsentVerificationResult.NotConfiguredForUser:
                            {
                                rootPage.NotifyUser("User has no fingeprints registered.", NotifyType.ErrorMessage);
                                break;
                            }

                        case UserConsentVerificationResult.RetriesExhausted:
                            {
                                rootPage.NotifyUser("Too many failed attempts.", NotifyType.ErrorMessage);
                                break;
                            }
                        case UserConsentVerificationResult.Canceled:
                            {
                                rootPage.NotifyUser("Consent request prompt was canceled.", NotifyType.ErrorMessage);
                                break;
                            }

                        default:
                            {
                                rootPage.NotifyUser("Consent verification with fingerprints is currently unavailable.", NotifyType.ErrorMessage);
                                break;
                            }
                    }
                }
                catch (Exception ex)
                {
                    rootPage.NotifyUser("Request current user's consent failed with exception. Operation: RequestVerificationAsync, Exception: " + ex.ToString(), NotifyType.ErrorMessage);
                }
                finally
                {
                    b.IsEnabled = true;
                }
            }
            else
            {
                rootPage.NotifyUser("Empty Message String. Enter prompt string in the Message text field.", NotifyType.ErrorMessage);
                b.IsEnabled = true;
            }
        }
    }
}
