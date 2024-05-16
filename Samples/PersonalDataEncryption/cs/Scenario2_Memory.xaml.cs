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

using System;
using Windows.Foundation;
using Windows.Security.Cryptography;
using Windows.Security.DataProtection;
using Windows.Storage.Streams;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario2_Memory : Page
    {
        private MainPage rootPage = MainPage.Current;
        UserDataProtectionManager userDataProtectionManager = null;
        IBuffer protectedBuffer = null;

        public Scenario2_Memory()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            userDataProtectionManager = UserDataProtectionManager.TryGetDefault();
            if (userDataProtectionManager != null)
            {
                AvailablePanel.Visibility = Visibility.Visible;
                UnavailablePanel.Visibility = Visibility.Collapsed;
            }
            else
            {
                AvailablePanel.Visibility = Visibility.Collapsed;
                UnavailablePanel.Visibility = Visibility.Visible;
            }
        }

        private async void ProtectL1_Click(object sender, RoutedEventArgs e)
        {
            string text = DataTextBox.Text;
            if (text.Length == 0)
            {
                // Cannot protect a zero-length buffer.
                rootPage.NotifyUser("Please enter data to protect", NotifyType.ErrorMessage);
                return;
            }

            IBuffer buffer = CryptographicBuffer.ConvertStringToBinary(text, BinaryStringEncoding.Utf8);

            IBuffer result = await userDataProtectionManager.ProtectBufferAsync(buffer, UserDataAvailability.AfterFirstUnlock);
            ReportProtectedBuffer(result);
        }

        private async void ProtectL2_Click(object sender, RoutedEventArgs e)
        {
            string text = DataTextBox.Text;
            if (text.Length == 0)
            {
                // Cannot protect a zero-length buffer.
                rootPage.NotifyUser("Please enter data to protect", NotifyType.ErrorMessage);
                return;
            }

            IBuffer buffer = CryptographicBuffer.ConvertStringToBinary(text, BinaryStringEncoding.Utf8);

            IBuffer result = await userDataProtectionManager.ProtectBufferAsync(buffer, UserDataAvailability.WhileUnlocked);
            ReportProtectedBuffer(result);
        }

        void ReportProtectedBuffer(IBuffer buffer)
        {
            protectedBuffer = buffer;
            ProtectedDataTextBlock.Text = CryptographicBuffer.EncodeToHexString(buffer);
            UnprotectPanel.Visibility = Visibility.Visible;
            rootPage.NotifyUser(string.Empty, NotifyType.StatusMessage);
        }

        private async void Unprotect_Click(object sender, RoutedEventArgs e)
        {
            UnprotectedDataTextBlock.Text = string.Empty;
            UserDataBufferUnprotectResult result = await userDataProtectionManager.UnprotectBufferAsync(protectedBuffer);
            switch (result.Status)
            {
                case UserDataBufferUnprotectStatus.Succeeded:
                    rootPage.NotifyUser("Unprotected successfully", NotifyType.StatusMessage);
                    UnprotectedDataTextBlock.Text = CryptographicBuffer.ConvertBinaryToString(BinaryStringEncoding.Utf8, result.UnprotectedBuffer);
                    break;

                case UserDataBufferUnprotectStatus.Unavailable:
                    rootPage.NotifyUser("Unable to unprotect: Unavailable", NotifyType.ErrorMessage);
                    break;

                default:
                    rootPage.NotifyUser("Unable to unprotect: Unknown error", NotifyType.ErrorMessage);
                    break;
            }
        }
    }
}
