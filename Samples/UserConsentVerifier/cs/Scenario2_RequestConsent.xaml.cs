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

namespace SDKTemplate
{
    public sealed partial class Scenario2_RequestConsent : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario2_RequestConsent()
        {
            this.InitializeComponent();
        }

        private async void RequestConsent_Click(object sender, RoutedEventArgs e)
        {
            RequestConsentButton.IsEnabled = false;

            // Request the user's consent using Windows Hello via biometric verification or a PIN.
            string message = "Please confirm your identity to complete this (pretend) in-app purchase.";
            UserConsentVerificationResult consentResult = await UserConsentVerifier.RequestVerificationAsync(message);
            switch (consentResult)
            {
                case UserConsentVerificationResult.Verified:
                    rootPage.NotifyUser("Pretend in-app purchase was successful.", NotifyType.StatusMessage);
                    break;

                case UserConsentVerificationResult.DeviceNotPresent:
                    rootPage.NotifyUser("No PIN or biometric found, please set one up.", NotifyType.ErrorMessage);
                    break;

                case UserConsentVerificationResult.Canceled:
                    rootPage.NotifyUser("User consent verification canceled.", NotifyType.ErrorMessage);
                    break;

                default:
                    rootPage.NotifyUser("User consent verification is currently unavailable.", NotifyType.ErrorMessage);
                    break;
            }
            RequestConsentButton.IsEnabled = true;
        }
    }
}
