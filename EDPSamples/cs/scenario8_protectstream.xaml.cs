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
    public sealed partial class Scenario8 : Page
    {
        private MainPage rootPage;
        InMemoryRandomAccessStream m_protectedStream = null;
        InMemoryRandomAccessStream m_unprotectedStream = null;
        private string m_protectStreamScenario = "An app has access to a stream of data that sources to an enterprise resource. " +
                                                 "In order to ensure that this stream is protected when transferring it within and outside the device, " +
                                                 "it protects the data to the enterprise identity it belongs to.This allows wiping the data when required by the enterprise.";

        private string m_getstreamStreamScenario = "\n\nAn app has previously protected a stream that it must prevent unauthorized access to.In order " +
                                                   "to retrieve its contents back when needed, it can check the status of the stream.Alternatively, " +
                                                   "if the app is denied access to the stream when unprotecting it, it can check its status.";


        public Scenario8()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            ScenarioText.Text = m_protectStreamScenario + m_getstreamStreamScenario;
            rootPage = MainPage.Current;
        }

        private async void UnprotectAsyncBuffer_Click(object sender, RoutedEventArgs e)
        {
            string outputStr = "";

            try
            {
                if (m_protectedStream != null)
                {
                    IInputStream source = m_protectedStream.GetInputStreamAt(0);
                    m_unprotectedStream = new InMemoryRandomAccessStream();
                    await DataProtectionManager.UnprotectStreamAsync(source,
                                                                     m_unprotectedStream
                                                                     );
                    DataReader unprotectedReader = new DataReader(m_unprotectedStream.GetInputStreamAt(0));
                    await unprotectedReader.LoadAsync((uint)m_unprotectedStream.Size);
                    IBuffer unprotectedStreamBuffer = unprotectedReader.ReadBuffer((uint)m_unprotectedStream.Size);
                    outputStr += "\n UnProtected Stream buffer:" +
                                   CryptographicBuffer.EncodeToHexString(unprotectedStreamBuffer);

                    rootPage.NotifyUser(outputStr, NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("Please protect a stream to unprotect", NotifyType.ErrorMessage);
                }
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser(outputStr + "\n" + ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        private async void GetStreamStatus_Click(object sender, RoutedEventArgs e)
        {
            string outputStr = "";

            try
            {
                if (m_protectedStream != null)
                {
                    DataProtectionInfo procStatus = await DataProtectionManager.GetStreamProtectionInfoAsync(m_protectedStream.GetInputStreamAt(0));
                    outputStr += "\n m_protectedStream protection status: " + procStatus.Status;
                    rootPage.NotifyUser(outputStr, NotifyType.StatusMessage);
                }
                else
                {
                    outputStr += "\nStream is null because ProtectStream API did not run successfully";
                    rootPage.NotifyUser(outputStr, NotifyType.ErrorMessage);
                }

            }
            catch (Exception ex)
            {
                rootPage.NotifyUser(outputStr + "\n" + ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        private async void ProtectStreamAsync_Click(object sender, RoutedEventArgs e)
        {
            string outputStr = "";
            Byte[] byteStream = { 1, 2, 3, 4, 5 };

            try
            {
                InMemoryRandomAccessStream randomAccessStream = new InMemoryRandomAccessStream();
                IOutputStream outputStream = randomAccessStream.GetOutputStreamAt(0);
                DataWriter writer = new DataWriter(outputStream);
                writer.WriteBytes(byteStream);
                await writer.StoreAsync();
                await writer.FlushAsync();

                IInputStream source = randomAccessStream.GetInputStreamAt(0);
                m_protectedStream = new InMemoryRandomAccessStream();
                IOutputStream destination = m_protectedStream.GetOutputStreamAt(0);

                await DataProtectionManager.ProtectStreamAsync(source,
                                                               Scenario1.m_EnterpriseIdentity,
                                                               destination
                                                               );

                DataReader reader = new DataReader(m_protectedStream.GetInputStreamAt(0));
                await reader.LoadAsync((uint)m_protectedStream.Size);
                IBuffer protectedStreamBuffer = reader.ReadBuffer((uint)m_protectedStream.Size);
                outputStr += "\n Protected Stream buffer:" + CryptographicBuffer.EncodeToHexString(protectedStreamBuffer);

                rootPage.NotifyUser(outputStr, NotifyType.StatusMessage);

            }
            catch (Exception ex)
            {
                rootPage.NotifyUser(outputStr + "\n" + ex.ToString(), NotifyType.ErrorMessage);
            }
        }
    }

}
