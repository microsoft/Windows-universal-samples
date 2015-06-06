//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Security.EnterpriseData;
using Windows.Security.Cryptography;
using SDKTemplate;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace EdpSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario7 : Page
    {
        private MainPage rootPage;
        private string m_protectedMessage = "Hello World";
        IBuffer m_protectedBuffer = null;
        IBuffer m_unprotectedBuffer = null;

        private string m_protectBufferScenario = "An app has retrieved sensitive data from a document that belongs to an enterprise. When " +
                                               "transferring this data within and outside the device, the app must ensure that there are no data " +
                                               "leaks. In order to do so, it protects the data to the enterprise identity it belongs to. This allows " +
                                               "wiping the data when required by the enterprise.";

        private string m_statusBufferScenario = "\n\nAn app has previously protected a buffer that it must prevent unauthorized access to. " +
                                               "In order to retrieve its contents back when needed, it can check the status of the buffer. " +
                                               "Alternatively, if the app is denied access to the buffer when unprotecting it, it can " +
                                               "check the status of the buffer.";



        public Scenario7()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            ScenarioTxtBlk.Text = m_protectBufferScenario + m_statusBufferScenario;
            rootPage = MainPage.Current;
        }

        private async void UnprotectAsyncBuffer_Click(object sender, RoutedEventArgs e)
        {
            string outputStr = "";
            string plainMessage = "";

            try
            {
                if (m_protectedBuffer != null)
                {
                    BufferProtectUnprotectResult unBuffer = await DataProtectionManager.UnprotectAsync(m_protectedBuffer);
                    m_unprotectedBuffer = unBuffer.Buffer;
                    plainMessage = CryptographicBuffer.ConvertBinaryToString(BinaryStringEncoding.Utf8,
                                                                             m_unprotectedBuffer
                                                                             );
                    outputStr += "\n Status: " + unBuffer.ProtectionInfo.Status;
                    outputStr += "\n Unprotected string:" + plainMessage;
                    rootPage.NotifyUser(outputStr, NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("Please protect a buffer to unprotect", NotifyType.ErrorMessage);
                }
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser(outputStr + "\n" + ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        private async void GetBuffersStatus_Click(object sender, RoutedEventArgs e)
        {
            string outputStr = "";

            try
            {
                if (m_protectedBuffer != null)
                {
                    DataProtectionInfo procStatus = await DataProtectionManager.GetProtectionInfoAsync(m_protectedBuffer);
                    outputStr += "\n protection status: " + procStatus.Status;
                    rootPage.NotifyUser(outputStr, NotifyType.StatusMessage);
                }
                else
                {
                    outputStr += "\n Buffer is null because Protect API did not run successfully";
                    rootPage.NotifyUser(outputStr, NotifyType.ErrorMessage);
                }

            }
            catch (Exception ex)
            {
                rootPage.NotifyUser(outputStr + "\n" + ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        private async void ProtectAsyncBuffer_Click(object sender, RoutedEventArgs e)
        {
            string outputStr = "";

            try
            {
                outputStr += "\n Protecting string:" + m_protectedMessage;
                IBuffer inputBuffer = CryptographicBuffer.ConvertStringToBinary(m_protectedMessage,
                                                                                BinaryStringEncoding.Utf8);
                BufferProtectUnprotectResult procBuffer = await DataProtectionManager.ProtectAsync(inputBuffer, Scenario1.m_EnterpriseIdentity);
                m_protectedBuffer = procBuffer.Buffer;
                outputStr += "\n Status: " + procBuffer.ProtectionInfo.Status;
                outputStr += "\n Protected string:";
                outputStr += CryptographicBuffer.EncodeToHexString(m_protectedBuffer).Substring(0, 20);
                rootPage.NotifyUser(outputStr, NotifyType.StatusMessage);

            }
            catch (Exception ex)
            {
                rootPage.NotifyUser(outputStr + "\n" + ex.ToString(), NotifyType.ErrorMessage);
            }
        }
    }

}
