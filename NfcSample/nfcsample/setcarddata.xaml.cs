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
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Security.Cryptography;
using Windows.Security.Cryptography.DataProtection;
using Windows.Storage.Streams;
using Windows.UI.Xaml.Controls;
using static NfcSample.NfcUtils;
using Windows.Devices.SmartCards;
using System.Runtime.InteropServices.WindowsRuntime;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace NfcSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class SetCardDataScenario : Page
    {
        private static byte[] AID_SAMPLE_CARD =
        {
            0xA0, 0x00, 0x00, 0x00, 0x03, 0x10, 0x10
        };

        public SetCardDataScenario()
        {
            this.InitializeComponent();
        }

        public async Task<IBuffer> ProtectDataAsync(
            byte[] data,
            String strDescriptor)
        {
            // Create a DataProtectionProvider object for the specified descriptor.
            DataProtectionProvider Provider = new DataProtectionProvider(strDescriptor);

            // Encrypt the message.
            return await Provider.ProtectAsync(CryptographicBuffer.CreateFromByteArray(data));
        }

        private byte[] BuildReadRecordResponse(string track2, string cardholderName, string track1Discretionary)
        {
            if (track2.Length % 2 != 0)
            {
                track2 += "F";
            }
            // Calculate the card length in bytes (will always be an even number because of padded 'F')
            int trackTwoLength = track2.Length / 2;

            // read record tag + size = 2, track 2 tag and size = 2, cardholder name tag and size = 3,
            // track 1 discretionary tag and size = 3, status word = 2
            List<byte> readRecordResponse = new List<byte>(trackTwoLength + cardholderName.Length + track1Discretionary.Length + 12);

            // Add the response header
            readRecordResponse.Add((byte)0x70); // Record template
            readRecordResponse.Add((byte)(2 + trackTwoLength + 3 + cardholderName.Length + 3 + track1Discretionary.Length)); // Data length plus size of tag and size
            readRecordResponse.Add((byte)0x57); // Track 2 equivalent data
            readRecordResponse.Add((byte)trackTwoLength); // Data length
            readRecordResponse.AddRange(NfcUtils.HexStringToBytes(track2));

            // Add the cardholder's name as ascii
            readRecordResponse.Add((byte)0x5F); // Tag cardholder name
            readRecordResponse.Add((byte)0x20); // Tag cardholder name
            readRecordResponse.Add((byte)cardholderName.Length); // Data length
            readRecordResponse.AddRange(System.Text.Encoding.UTF8.GetBytes(cardholderName));

            // Add track one discretionary data as ascii
            readRecordResponse.Add((byte)0x9F);
            readRecordResponse.Add((byte)0x1F);
            readRecordResponse.Add((byte)track1Discretionary.Length);
            readRecordResponse.AddRange(System.Text.Encoding.UTF8.GetBytes(track1Discretionary));

            // Add the result code
            readRecordResponse.Add((byte)0x90); // Status byte one (SW1)
            readRecordResponse.Add((byte)0x00); // Status byte two (SW2)

            return readRecordResponse.ToArray();
        }

        private void btnCancel_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            this.Frame.GoBack();
        }

        private async void btnAddCard_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            // Clear the messages
            MainPage.Current.NotifyUser(String.Empty, NotifyType.StatusMessage, true);

            // Register the AID/applet IDs that this application will handle for our particular card(s)
            var reg = await NfcUtils.RegisterAidGroup(txtDisplayName.Text, new[] { AID_PPSE.AsBuffer(), AID_SAMPLE_CARD.AsBuffer() }, SmartCardEmulationCategory.Payment, SmartCardEmulationType.Host, (chkAutomaticEnablement.IsChecked ?? false));
            
            var track2 = txtPAN.Text + "D" + txtExpiryYear.Text + txtExpiryMonth.Text + txtServiceCode.Text + txtTrack2Discretionary.Text;
            var record = BuildReadRecordResponse(track2, txtCardholderName.Text, txtTrack1Discretionary.Text);
            var encryptedRecord = await ProtectDataAsync(record, "LOCAL=user");
            
            var file = await Windows.Storage.ApplicationData.Current.LocalFolder.CreateFileAsync("ReadRecordResponse-" + reg.Id.ToString("B") + ".dat", Windows.Storage.CreationCollisionOption.ReplaceExisting);
            await Windows.Storage.FileIO.WriteBufferAsync(file, encryptedRecord);

            LogMessage("Card data saved", NotifyType.StatusMessage);
            this.Frame.GoBack();
        }
    }
}
