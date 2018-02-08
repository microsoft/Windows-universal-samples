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
    public sealed partial class Scenario1_CheckConsentAvailability : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario1_CheckConsentAvailability()
        {
            this.InitializeComponent();
        }

        private async void CheckAvailability_Click(object sender, RoutedEventArgs e)
        {
            CheckAvailabilityButton.IsEnabled = false;

            // Check the availability of Windows Hello authentication through User Consent Verifier.
            UserConsentVerifierAvailability consentAvailability = await UserConsentVerifier.CheckAvailabilityAsync();
            switch (consentAvailability)
            {
                case UserConsentVerifierAvailability.Available:
                    rootPage.NotifyUser("User consent verification available!", NotifyType.StatusMessage);
                    break;

                case UserConsentVerifierAvailability.DeviceNotPresent:
                    rootPage.NotifyUser("No PIN or biometric found, please set one up.", NotifyType.ErrorMessage);
                    break;

                default:
                    rootPage.NotifyUser("User consent verification is currently unavailable.", NotifyType.ErrorMessage);
                    break;
            }

            CheckAvailabilityButton.IsEnabled = true;
        }
    }
}

