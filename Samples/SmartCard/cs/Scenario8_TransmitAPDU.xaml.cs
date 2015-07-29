using SDKTemplate;

using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.Storage.Streams;
using Windows.Security.Cryptography;
using Windows.Devices.SmartCards;

namespace Smartcard
{
    public sealed partial class Scenario8_TransmitAPDU : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario8_TransmitAPDU()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Click handler for the 'TransmitAPDU' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void Transmit_Click(object sender, RoutedEventArgs e)
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

                IBuffer result = null;

                using (SmartCardConnection connection = await card.ConnectAsync())
                {
                    // Read EF.ATR file
                    // The command is meant specifically for GIDS cards 
                    // (such as TPM VSCs), and will fail on other types.
                    byte[] readEfAtrBytes = { 0x00, 0xCB, 0x2F, 0x01, 0x02, 0x5C, 0x00, 0xFF };

                    IBuffer readEfAtr = CryptographicBuffer.CreateFromByteArray(readEfAtrBytes);

                    result = await connection.TransmitAsync(readEfAtr);
                
                    rootPage.NotifyUser("Response: " + CryptographicBuffer.EncodeToHexString(result), NotifyType.StatusMessage);
                }
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Transmitting APDU to card failed with exception: " + ex.ToString(), NotifyType.ErrorMessage);
            }
            finally
            {
                b.IsEnabled = true;
            }
        }
    }
}
