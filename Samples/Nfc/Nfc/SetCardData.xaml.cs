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

using Iso7816;
using SDKTemplate;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Devices.SmartCards;
using Windows.Security.Cryptography;
using Windows.Security.Cryptography.DataProtection;
using Windows.Storage.Streams;
using Windows.UI.Xaml.Controls;
using static NfcSample.NfcUtils;

namespace NfcSample
{
    public sealed partial class SetCardDataScenario : Page
    {
        private static readonly byte[] AID_PPSE =
        {
            // File name "2PAY.SYS.DDF01" (14 bytes)
            (byte)'2', (byte)'P', (byte)'A', (byte)'Y', (byte)'.',
            (byte)'S', (byte)'Y', (byte)'S', (byte)'.',
            (byte)'D', (byte)'D', (byte)'F', (byte)'0', (byte)'1'
        };

        private static readonly byte[] AID_V =
        {
            0xA0, 0x00, 0x00, 0x00, 0x03, 0x10, 0x10
        };

        private static readonly byte[] AID_MC =
        {
            0xA0, 0x00, 0x00, 0x00, 0x04, 0x10, 0x10
        };

        private static readonly byte[] AID_NONPAYMENT =
        {
            0x12, 0x34, 0x56, 0x78, 0x90
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

        private byte[] BuildReadRecordResponseV(string track2, string cardholderName, string track1Discretionary)
        {
            if (track2.Length % 2 != 0)
            {
                track2 += "F";
            }

            return new TlvEntry(0x70, new TlvEntry[] {
                new TlvEntry(0x57, NfcUtils.HexStringToBytes(track2)),
                new TlvEntry(0x5F20, cardholderName),
                new TlvEntry(0x9F1F, track1Discretionary),
                }).GetData(0x9000);
        }

        private byte[] BuildReadRecordResponseMC(string pan, string exp, string serviceCode, string cardholderName, string track1Discretionary, string track2Discretionary)
        {
            var track1 = "B" + pan + "^" + cardholderName + "^" + exp + serviceCode + track1Discretionary;
            var track2 = pan + "D" + exp + serviceCode + track2Discretionary;
            if (track2.Length % 2 != 0)
            {
                track2 += "F";
            }

            return new TlvEntry(0x70, new TlvEntry[] {
                // Magstripe version
                new TlvEntry(0x9F6C, new byte[] { 0x00, 0x01 }),

                // Track 1 data
                new TlvEntry(0x56, System.Text.Encoding.UTF8.GetBytes(track1)),
                
                // Track 1 bit map for CVC3
                new TlvEntry(0x9F62,  new byte[] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E }),

                // Track 1 bit map for UN and ATC
                new TlvEntry(0x9F63, new byte[] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }),

                // Track 1 number of ATC digits
                new TlvEntry(0x9F64, new byte[] { 0x00 }),
                
                // Track 2 data
                new TlvEntry(0x9F6B, NfcUtils.HexStringToBytes(track2)),
                
                // Track 2 bit map for CVC3
                new TlvEntry(0x9F65, new byte[] { 0x00, 0x0E }),

                // Track 2 bit map for UN and ATC
                new TlvEntry(0x9F66,  new byte[] { 0x00, 0x00 }),

                // Track 2 number of ATC digits
                new TlvEntry(0x9F67, new byte[] { 0x00 }),
                
                // Magstripe CVM list
                new TlvEntry(0x9F68, new byte[] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5E, 0x03, 0x42, 0x03, 0x1F, 0x03 }),
                }).GetData(0x9000);
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
            SmartCardAppletIdGroupRegistration reg;

            if (optUICC.IsChecked ?? false)
            {
                reg = await NfcUtils.RegisterAidGroup(
                    txtDisplayName.Text,
                    new[] { AID_PPSE.AsBuffer(), AID_V.AsBuffer(), AID_MC.AsBuffer() },
                    SmartCardEmulationCategory.Payment,
                    SmartCardEmulationType.Uicc,
                    (chkAutomaticEnablement.IsChecked ?? false));
            }
            else if (optNonPayment.IsChecked ?? false)
            {
                reg = await NfcUtils.RegisterAidGroup(
                    txtDisplayName.Text,
                    new[] { AID_NONPAYMENT.AsBuffer() },
                    SmartCardEmulationCategory.Other,
                    SmartCardEmulationType.Host,
                    (chkAutomaticEnablement.IsChecked ?? false));

                var rule1 = new SmartCardAutomaticResponseApdu(NfcUtils.HexStringToBytes("0012AABB").AsBuffer(), NfcUtils.HexStringToBytes("AABBCCDDEE9000").AsBuffer());
                rule1.AppletId = AID_NONPAYMENT.AsBuffer();
                await reg.SetAutomaticResponseApdusAsync(new List<SmartCardAutomaticResponseApdu>() { rule1 });
            }
            else
            {
                var aid = (optV.IsChecked ?? false) ? AID_V : AID_MC;
                var aidBuffer = aid.AsBuffer();
                reg = await NfcUtils.RegisterAidGroup(
                    txtDisplayName.Text,
                    new[] { AID_PPSE.AsBuffer(), aidBuffer },
                    SmartCardEmulationCategory.Payment,
                    SmartCardEmulationType.Host,
                    (chkAutomaticEnablement.IsChecked ?? false));

                var rules = new List<SmartCardAutomaticResponseApdu>();

                // Construct SELECT PPSE response and set as auto responder
                rules.Add(new SmartCardAutomaticResponseApdu(
                    new SelectCommand(AID_PPSE, 0x00).GetBuffer(),
                    new TlvEntry(0x6F, new TlvEntry[]
                    {
                        new TlvEntry(0x84, "2PAY.SYS.DDF01"),
                        new TlvEntry(0xA5, new TlvEntry[]
                        {
                            new TlvEntry(0xBF0C,
                                new TlvEntry(0x61, new TlvEntry[]
                                {
                                    new TlvEntry(0x4F, aidBuffer),
                                    new TlvEntry(0x87, new byte[] { 0x01 })
                                }))
                        })
                    }).GetData(0x9000).AsBuffer()));

                if (optV.IsChecked ?? false)
                {
                    rules.Add(new SmartCardAutomaticResponseApdu(
                        new SelectCommand(aid, 0x00).GetBuffer(),
                        new TlvEntry(0x6F, new TlvEntry[]
                        {
                            new TlvEntry(0x84, aidBuffer),
                            new TlvEntry(0xA5, new TlvEntry[]
                            {
                                new TlvEntry(0x50, "CREDIT CARD"),
                                new TlvEntry(0x9F38, new byte[] { 0x9F, 0x66, 0x02 })
                            })
                        }).GetData(0x9000).AsBuffer()));

                    var ruleGpo = new SmartCardAutomaticResponseApdu(
                        NfcUtils.HexStringToBytes("80A80000").AsBuffer(),
                        new TlvEntry(0x80, new byte[] { 0x00, 0x80, 0x08, 0x01, 0x01, 0x00 }).GetData(0x9000).AsBuffer());
                    ruleGpo.AppletId = aidBuffer;
                    ruleGpo.ShouldMatchLength = false;
                    rules.Add(ruleGpo);
                }
                else
                {
                    rules.Add(new SmartCardAutomaticResponseApdu(
                        new SelectCommand(aid, 0x00).GetBuffer(),
                        new TlvEntry(0x6F, new TlvEntry[]
                        {
                            new TlvEntry(0x84, aidBuffer),
                            new TlvEntry(0xA5, new TlvEntry[]
                            {
                                new TlvEntry(0x50, "CREDIT CARD")
                            })
                        }).GetData(0x9000).AsBuffer()));

                    var ruleGpo = new SmartCardAutomaticResponseApdu(
                        NfcUtils.HexStringToBytes("80A80000").AsBuffer(),
                        new TlvEntry(0x77, new TlvEntry[]
                        {
                            new TlvEntry(0x82, new byte[] { 0x00, 0x00 }),
                            new TlvEntry(0x94, new byte[] { 0x08, 0x01, 0x01, 0x00 }),
                            new TlvEntry(0xD7, new byte[] { 0x00, 0x00, 0x80 })
                        }).GetData(0x9000).AsBuffer());
                    ruleGpo.AppletId = aidBuffer;
                    ruleGpo.ShouldMatchLength = false;
                    rules.Add(ruleGpo);
                }

                byte[] record;
                if (optV.IsChecked ?? false)
                {
                    var track2 = txtPAN.Text + "D" + txtExpiryYear.Text + txtExpiryMonth.Text + txtServiceCode.Text + txtTrack2Discretionary.Text;
                    record = BuildReadRecordResponseV(track2, txtCardholderName.Text, txtTrack1Discretionary.Text);
                }
                else
                {
                    record = BuildReadRecordResponseMC(txtPAN.Text, txtExpiryYear.Text + txtExpiryMonth.Text, txtServiceCode.Text, txtCardholderName.Text, txtTrack1Discretionary.Text, txtTrack2Discretionary.Text);
                }

                var encryptedRecord = await ProtectDataAsync(record, "LOCAL=user");

                var file = await Windows.Storage.ApplicationData.Current.LocalFolder.CreateFileAsync("ReadRecordResponse-" + reg.Id.ToString("B") + ".dat", Windows.Storage.CreationCollisionOption.ReplaceExisting);
                await Windows.Storage.FileIO.WriteBufferAsync(file, encryptedRecord);

                await reg.SetAutomaticResponseApdusAsync(rules);
            }

            LogMessage("Card data saved", NotifyType.StatusMessage);
            this.Frame.GoBack();
        }
        
        private void optCardTypeGroup_CheckedUnchecked(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (stackPayment != null)
            {
                if (optNonPayment != null && (optNonPayment.IsChecked ?? false))
                {
                    txtDisplayName.Text = "Sample Other/Non-Payment Card";
                    stackPayment.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
                }
                else if (optUICC != null && (optUICC.IsChecked ?? false))
                {
                    txtDisplayName.Text = "Sample UICC Payment Card";
                    stackPayment.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
                }
                else
                {
                    txtDisplayName.Text = "Sample Payment Card";
                    stackPayment.Visibility = Windows.UI.Xaml.Visibility.Visible;
                }
            }
        }
    }
}
