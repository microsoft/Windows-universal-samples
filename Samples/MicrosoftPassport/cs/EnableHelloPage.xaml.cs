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
// Please refer to the Microsoft Passport and Windows Hello
// whitepaper on the Windows Dev Center for a complete
// explanation of Microsoft Passport and Windows Hello
// implementation: 
// http://go.microsoft.com/fwlink/p/?LinkId=522066
//*********************************************************
using System;
using System.Linq;
using System.Threading.Tasks;
using Windows.Data.Json;
using Windows.Networking.Connectivity;
using Windows.Security.Credentials;
using Windows.Security.Cryptography;
using Windows.Security.Cryptography.Core;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class EnableHelloPage : Page
    {
        private string userId;

        public EnableHelloPage()
        {
            InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            userId = e.Parameter.ToString();
            WelcomeUserId.Text = userId;
        }

        private void SkipSetup()
        {
            // The user already signed in on the previous page, so we can go directly to the signed-in state.
            Frame.Navigate(typeof(AccountOverviewPage));
        }

        private async void StartUsingWindowsHello()
        {
            ProgressRing.IsActive = true;
            LaterButton.IsEnabled = false;
            StartUsingWindowsHelloButton.IsEnabled = false;

            // Create the key credential with Passport APIs
            IBuffer publicKey = await CreatePassportKeyCredentialAsync();
            if (publicKey != null)
            {
                // Register the public key and attestation of the key credential with the server
                // In a real-world scenario, this would likely also include:
                // - Certificate chain for attestation endorsement if available
                // - Status code of the Key Attestation result : Included / retrieved later / retry type
                if (await RegisterPassportCredentialWithServerAsync(publicKey))
                {
                    // Remember that this is the user whose credentials have been registered
                    // with the server.
                    ApplicationData.Current.LocalSettings.Values["userId"] = userId;

                    // When communicating with the server in the future, we pass a hash of the
                    // public key in order to identify which key the server should use to verify the challenge.
                    HashAlgorithmProvider hashProvider = HashAlgorithmProvider.OpenAlgorithm(HashAlgorithmNames.Sha256);
                    IBuffer publicKeyHash = hashProvider.HashData(publicKey);
                    ApplicationData.Current.LocalSettings.Values["publicKeyHint"] = CryptographicBuffer.EncodeToBase64String(publicKeyHash);

                    // Registration successful. Continue to the signed-in state.
                    Frame.Navigate(typeof(AccountOverviewPage));
                }
                else
                {
                    // Delete the failed credentials from the device.
                    await Util.TryDeleteCredentialAccountAsync(userId);

                    MessageDialog message = new MessageDialog("Failed to register with the server.");
                    await message.ShowAsync();
                }
            }
            ProgressRing.IsActive = false;
            LaterButton.IsEnabled = true;
            StartUsingWindowsHelloButton.IsEnabled = true;
        }

        private async Task<IBuffer> CreatePassportKeyCredentialAsync()
        {
            // Create a new KeyCredential for the user on the device.
            KeyCredentialRetrievalResult keyCreationResult = await KeyCredentialManager.RequestCreateAsync(userId, KeyCredentialCreationOption.ReplaceExisting);
            if (keyCreationResult.Status == KeyCredentialStatus.Success)
            {
                // User has autheniticated with Windows Hello and the key credential is created.
                KeyCredential userKey = keyCreationResult.Credential;
                return userKey.RetrievePublicKey();
            }
            else if (keyCreationResult.Status == KeyCredentialStatus.NotFound)
            {
                MessageDialog message = new MessageDialog("To proceed, Windows Hello needs to be configured in Windows Settings (Accounts -> Sign-in options)");
                await message.ShowAsync();

                return null;
            }
            else if (keyCreationResult.Status == KeyCredentialStatus.UnknownError)
            {
                MessageDialog message = new MessageDialog("The key credential could not be created. Please try again.");
                await message.ShowAsync();

                return null;
            }

            return null;
        }


        // Register the user and device with the server
        private async Task<bool> RegisterPassportCredentialWithServerAsync(IBuffer publicKey)
        {
            // Include the name of the current device for the benefit of the user.
            // The server could support a Web interface that shows the user all the devices they
            // have signed in from and revoke access from devices they have lost.

            var hostNames = NetworkInformation.GetHostNames();
            var localName = hostNames.FirstOrDefault(name => name.DisplayName.Contains(".local"));
            string computerName = localName.DisplayName.Replace(".local", "");

            JsonValue jsonResult = await JsonRequest.Create()
                .AddString("userId", userId)
                .AddString("publicKey", CryptographicBuffer.EncodeToBase64String(publicKey))
                .AddString("deviceName", computerName)
                .PostAsync("api/Authentication/Register");
            bool result = (jsonResult != null) && jsonResult.GetBoolean();
            
            return result;
        }

        private async void LaunchHyperlink(object sender, RoutedEventArgs e)
        {
            var element = (FrameworkElement)sender;
            var target = (string)element.Tag;
            await Windows.System.Launcher.LaunchUriAsync(new Uri(target));

        }
    }
}
