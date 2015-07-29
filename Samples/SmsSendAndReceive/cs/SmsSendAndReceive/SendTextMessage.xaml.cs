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
using SDKTemplate;
using System;
using System.Collections.Generic;
using Windows.Devices.Sms;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SmsSendAndReceive
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class SendTextMessage : Page
    {
        private MainPage rootPage;
        private SmsDevice2 device;

        public SendTextMessage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private async void Send_Click(object sender, RoutedEventArgs e)
        {
            if (SendToText.Text == "")
            {
                rootPage.NotifyUser("Please enter sender number", NotifyType.ErrorMessage);
                return;
            }

            if (SendMessageText.Text == "")
            {
                rootPage.NotifyUser("Please enter message", NotifyType.ErrorMessage);
                return;
            }

            // If this is the first request, get the default SMS device. If this
            // is the first SMS device access, the user will be prompted to grant
            // access permission for this application.
            if (device == null)
            {
                try
                {
                    rootPage.NotifyUser("Getting default SMS device ...", NotifyType.StatusMessage);
                    device = SmsDevice2.GetDefault();
                }
                catch (Exception ex)
                {
                    rootPage.NotifyUser(ex.Message, NotifyType.ErrorMessage);
                    return;
                }
                
            }

            string msgStr = "";
            if (device != null)
            {
                try
                {
                    // Create a text message - set the entered destination number and message text.
                    SmsTextMessage2 msg = new SmsTextMessage2();
                    msg.To = SendToText.Text;
                    msg.Body = SendMessageText.Text;

                    // Send the message asynchronously
                    rootPage.NotifyUser("Sending message ...", NotifyType.StatusMessage);
                    SmsSendMessageResult result = await device.SendMessageAndGetResultAsync(msg);

                    if (result.IsSuccessful)
                    {
                        msgStr = "";
                        msgStr += "Text message sent, cellularClass: " + result.CellularClass.ToString();
                        IReadOnlyList<Int32> messageReferenceNumbers = result.MessageReferenceNumbers;

                        for (int i = 0; i < messageReferenceNumbers.Count; i++)
                        {
                            msgStr += "\n\t\tMessageReferenceNumber[" + i.ToString() + "]: " + messageReferenceNumbers[i].ToString();
                        }
                        rootPage.NotifyUser(msgStr, NotifyType.StatusMessage);
                    }
                    else
                    {
                        msgStr = "";
                        msgStr += "ModemErrorCode: " + result.ModemErrorCode.ToString();
                        msgStr += "\nIsErrorTransient: " + result.IsErrorTransient.ToString();
                        if (result.ModemErrorCode == SmsModemErrorCode.MessagingNetworkError)
                        {                            
                            msgStr += "\n\tNetworkCauseCode: " + result.NetworkCauseCode.ToString();

                            if (result.CellularClass == CellularClass.Cdma)
                            {
                                msgStr += "\n\tTransportFailureCause: " + result.TransportFailureCause.ToString();
                            }
                            rootPage.NotifyUser(msgStr, NotifyType.ErrorMessage);
                        }
                    }
                }
                catch (Exception ex)
                {
                    rootPage.NotifyUser(ex.Message, NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("Failed to find SMS device", NotifyType.ErrorMessage);
            }
        }
    }
}
