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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace Smartcard
{
    public sealed partial class Scenario2_ChangePIN : Page
    {
        private MainPage rootPage = MainPage.Current;
        
        public Scenario2_ChangePIN()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Click handler for the 'ChangePin' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void ChangePin_Click(object sender, RoutedEventArgs e)
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

                rootPage.NotifyUser("Changing smart card PIN...", NotifyType.StatusMessage);

                bool result = await provisioning.RequestPinChangeAsync();

                if (result)
                {
                    rootPage.NotifyUser("Smart card change PIN operation completed.", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("Smart card change PIN operation was canceled by the user.", NotifyType.StatusMessage);
                }
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Changing smart card PIN failed with exception: " + ex.ToString(), NotifyType.ErrorMessage);
            }
            finally
            {
                b.IsEnabled = true;
            }
        }
    }
}
