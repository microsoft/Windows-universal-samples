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
using System.Threading.Tasks;
using Windows.Data.Json;
using Windows.Security.Credentials;
using Windows.Security.Cryptography;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class MainPage : Page
    {
        private string userId;
        private string publicKeyHint;

        public MainPage()
        {
            InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            userId = ApplicationData.Current.LocalSettings.Values["userId"] as string ?? string.Empty;
            publicKeyHint = ApplicationData.Current.LocalSettings.Values["publicKeyHint"] as string ?? string.Empty;

            UpdateUI();
        }

        private void UpdateUI()
        {
            RegisteredUserName.Text = userId;
            if (userId.Length != 0 && publicKeyHint.Length != 0)
            {
                SignInWithHelloContent.Visibility = Visibility.Visible;
                SignInWithPasswordContent.Visibility = Visibility.Collapsed;
            }
            else
            {
                SignInWithHelloContent.Visibility = Visibility.Collapsed;
                SignInWithPasswordContent.Visibility = Visibility.Visible;
            }
        }

        private async void Unregister()
        {
            // Don't let the user try a new operation while we are busy with this one.
            SignInWithHelloButton.IsEnabled = false;
            SignInAsSomeoneElseButton.IsEnabled = false;

            // Remove the credential from the key credential manager, to free up space on the device.
            await Util.TryDeleteCredentialAccountAsync(userId);

            // Remove the credential from the server, to free up space on the server.
            JsonValue jsonResult = await JsonRequest.Create()
                .AddString("userId", userId)
                .AddString("publicKeyHint", publicKeyHint)
                .PostAsync("api/Authentication/RemoveRegisteredKey");
            if (jsonResult == null || !jsonResult.GetBoolean())
            {
                // Could not delete from the server. Nothing we can do; just ignore the error.
            }

            // Remove our app's knowledge of the user.
            ApplicationData.Current.LocalSettings.Values.Remove("userId");
            ApplicationData.Current.LocalSettings.Values.Remove("publicKeyHint");
            userId = string.Empty;

            SignInWithHelloButton.IsEnabled = true;
            SignInAsSomeoneElseButton.IsEnabled = true;

            UpdateUI();
        }

        // Signing in requires a non-blank user name and password.
        private void OnSignInInfoChanged()
        {
            SignInWithPasswordButton.IsEnabled = (!string.IsNullOrWhiteSpace(UserNameTextBox.Text) && !string.IsNullOrWhiteSpace(PasswordTextBox.Password));
        }

        private async void SignInWithPassword()
        {
            string password = PasswordTextBox.Password;

            // Perform traditional standard authentication here.
            // Our sample accepts any userid and password.

            // Next, see if we can offer to switch to Windows Hello sign-in.
            // Check if the device is capable of provisioning Microsoft Passport key credentials and
            // the user has set up Windows Hello with a PIN on the device.
            if (await KeyCredentialManager.IsSupportedAsync())
            {
                // Navigate to Enable Hello Page, passing the account ID (username) as a parameter
                string accountID = UserNameTextBox.Text;
                Frame.Navigate(typeof(EnableHelloPage), accountID);
            }
            else
            {
                // Windows Hello is not available, so go directly to the signed-in state.
                // For the purpose of this sample, we will display a message to indicate
                // that this code path has been reached.
                MessageDialog message = new MessageDialog("Sample note: Since Windows Hello is not set up, we will go directly to the signed-in state.");
                await message.ShowAsync();
                Frame.Navigate(typeof(AccountOverviewPage));
            }
        }

        private async Task<bool> SignInWithHelloAsync()
        {
            // Open the existing user key credential.
            KeyCredentialRetrievalResult retrieveResult = await KeyCredentialManager.OpenAsync(userId);

            if (retrieveResult.Status != KeyCredentialStatus.Success)
            {
                return false;
            }

            KeyCredential userCredential = retrieveResult.Credential;

            // Request a challenge from the server.
            JsonValue jsonChallenge = await JsonRequest.Create()
                .AddString("userId", userId)
                .AddString("publicKeyHint", publicKeyHint)
                .PostAsync("api/Authentication/RequestChallenge");
            if (jsonChallenge == null)
            {
                return false;
            }

            // Sign the challenge using the user's KeyCredential.
            IBuffer challengeBuffer = CryptographicBuffer.DecodeFromBase64String(jsonChallenge.GetString());
            KeyCredentialOperationResult opResult = await userCredential.RequestSignAsync(challengeBuffer);

            if (opResult.Status != KeyCredentialStatus.Success)
            {
                return false;
            }

            // Get the signature.
            IBuffer signatureBuffer = opResult.Result;

            // Send the signature back to the server to confirm our identity.
            // The publicKeyHint tells the server which public key to use to verify the signature.
            JsonValue jsonResult = await JsonRequest.Create()
                .AddString("userId", userId)
                .AddString("publicKeyHint", publicKeyHint)
                .AddString("signature", CryptographicBuffer.EncodeToBase64String(signatureBuffer))
                .PostAsync("api/Authentication/SubmitResponse");
            if (jsonResult == null)
            {
                return false;
            }

            return jsonResult.GetBoolean();
        }

        private async void SignInWithHello()
        {
            ProgressRing.IsActive = true;

            // Don't let the user try a new operation while we are busy with this one.
            SignInWithHelloButton.IsEnabled = false;
            SignInAsSomeoneElseButton.IsEnabled = false;

            bool result = await SignInWithHelloAsync();

            ProgressRing.IsActive = false;
            SignInWithHelloButton.IsEnabled = true;
            SignInAsSomeoneElseButton.IsEnabled = true;

            if (result)
            {
                Frame.Navigate(typeof(AccountOverviewPage));
            }
            else
            {
                MessageDialog message = new MessageDialog("Login with Windows Hello failed.");
                await message.ShowAsync();
            }
        }
    }
}
