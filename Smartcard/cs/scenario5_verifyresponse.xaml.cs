using SDKTemplate;
using System;
using Windows.Devices.SmartCards;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace Smartcard
{
    public sealed partial class Scenario5_VerifyResponse : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario5_VerifyResponse()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Click handler for the 'VerifyResponse' button. 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void VerifyResponse_Click(object sender, RoutedEventArgs e)
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
                bool verifyResult = false;
                SmartCard card = await rootPage.GetSmartCard();
                SmartCardProvisioning provisioning = await SmartCardProvisioning.FromSmartCardAsync(card);

                rootPage.NotifyUser("Verifying smart card response...", NotifyType.StatusMessage);

                using (SmartCardChallengeContext context = await provisioning.GetChallengeContextAsync())
                {
                    IBuffer response = ChallengeResponseAlgorithm.CalculateResponse(context.Challenge, rootPage.AdminKey);
                    verifyResult = await context.VerifyResponseAsync(response);
                }

                rootPage.NotifyUser("Smart card response verification completed. Result: " + verifyResult.ToString(), NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser( "Verifying smart card response operation failed with exception: " + ex.ToString(), NotifyType.ErrorMessage);
            }
            finally
            {
                b.IsEnabled = true;
            }
        }
    }
}
