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

using SDKTemplate;
using System;
using Windows.Devices.SmartCards;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace Smartcard
{
    public sealed partial class Scenario3_ResetPIN : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario3_ResetPIN()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Click handler for the 'ResetPin' button. 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void ResetPin_Click(object sender, RoutedEventArgs e)
        {
            if (!rootPage.ValidateTPMSmartCard())
            {
                rootPage.NotifyUser("Use Scenario One to create a TPM virtual smart card.", NotifyType.ErrorMessage);
                return;
            }

            Button b = sender as Button;
            b.IsEnabled = false;

            try
            {
                SmartCard card = await rootPage.GetSmartCard();
                SmartCardProvisioning provisioning = await SmartCardProvisioning.FromSmartCardAsync(card);

                rootPage.NotifyUser("Resetting smart card PIN...", NotifyType.StatusMessage);

                // When requesting a PIN reset, a SmartCardPinResetHandler must be
                // provided as an argument.  This handler must use the challenge
                // it receives and the card's admin key to calculate and set the
                // response.
                bool result = await provisioning.RequestPinResetAsync(
                    (pinResetSender, request) =>
                    {
                        SmartCardPinResetDeferral deferral = request.GetDeferral();

                        try
                        {
                            IBuffer response = ChallengeResponseAlgorithm.CalculateResponse(request.Challenge, rootPage.AdminKey);
                            request.SetResponse(response);
                        }
                        finally
                        {
                            deferral.Complete();
                        }
                    });

                if (result)
                {
                    rootPage.NotifyUser("Smart card PIN reset operation completed.", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("Smart card PIN reset operation was canceled by the user.", NotifyType.StatusMessage);
                }

            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Resetting smart card PIN failed with exception: " + ex.ToString(), NotifyType.ErrorMessage);
            }
            finally
            {
                b.IsEnabled = true;
            }
        }
    }
}
