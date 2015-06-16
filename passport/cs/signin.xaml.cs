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
using SDKTemplate;
using Windows.Security.Credentials;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Media.Animation;
using Windows.UI;
using Windows.UI.Xaml.Media;
using System.Threading.Tasks;
using Windows.Storage.Streams;
using System.Text;
using System.IO;
using Windows.Security.Cryptography;

namespace SDKTemplate
{
    /// <summary>
    /// The sign in form that contains username and password fields and the option to sign in
    /// with either Microsoft Passport or traditional username/password authentication.
    /// 
	/// Right now there is no authentication of username/password except for that they are not empty -
	/// so any username/password combo will be accepted. However if Passport is used, it will use the
	/// actual Passport PIN that is set up on the machine running the sample.
	///
    /// If an account was passed from the user select we will do one of two things
    /// 
    ///     1. If the selected account used Microsoft Passport as the last sign in method, it will attempt Microsoft Passport
    ///         sign in first. If that is cancelled or fails, it will fall back on username/password form for sign in.
    /// 
    ///     2. Else - show the username/password sign in form with username filled in based on chosen account.
    /// </summary>
    public sealed partial class SignIn : Page
    {
        private MainPage rootPage;
        private bool m_addingAccount = true;
        private Account m_account;
        private bool m_passportAvailable = true;

        public SignIn()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// The function that will be called when this frame is navigated to.
        ///
        /// Checks to see if Microsoft Passport is available and checks to see if an 
        /// account was passed in.
        ///
        /// If an account was passed, check if it uses Microsoft Passport.
        /// and set the "adding user" flag so we don't add a new account
        /// to the list of users.
        /// </summary>
        /// <param name="e"></param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            PassportAvailableCheck();

            if (e.Parameter != null)
            {
                m_account = (Account)e.Parameter;
                textbox_Username.Text = m_account.Email;
                textbox_Username.IsEnabled = false;
                m_addingAccount = false;

                if (m_account.UsesPassport == true)
                {
                    SignInPassport(true);
                }
            }
        }

        /// <summary>
        /// Function called when regular username/password sign in button is clicked
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Button_SignIn_Click(object sender, RoutedEventArgs e)
        {
            SignInPassword(false);
        }

        /// <summary>
        /// Function called when the "Sign In with Microsoft Passport" button is clicked.
        /// 
        /// Checks if we are using an existing account and passes their preference for passport 
        /// otherwise if this is a new account then they don't have a preference yet.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Button_Passport_Click(object sender, RoutedEventArgs e)
        {
            if (m_addingAccount == false)
            {
                SignInPassport(m_account.UsesPassport);
            }
            else
            {
                SignInPassport(false);
            }
        }

        /// <summary>
        /// Checks to see if Passport is ready to be used.
        /// 
        /// Passport has dependencies on:
        ///     1. Having a connected MSA Account
        ///     2. Having a Windows PIN set up for that account on the local machine
        ///
        /// If Passport is not ready, then display how to set it up to the user and 
        /// disable the "Sign In with Passport" button.
        /// </summary>
        private async void PassportAvailableCheck()
        {
            var keyCredentialAvailable = await KeyCredentialManager.IsSupportedAsync();

            if (keyCredentialAvailable == false)
            {
                //
                // Key credential is not enabled yet as user 
                // needs to connect to a MSA account and select a pin in the connecting flow.
                //
                grid_PassportStatus.Background = new SolidColorBrush(Color.FromArgb(255, 50, 170, 207));
                textblock_PassportStatusText.Text = "Microsoft Passport is not set up.\nPlease go to Windows Settings and connect an MSA account!";
                button_PassportSignIn.IsEnabled = false;
                m_passportAvailable = false;
            }
        }

        /// <summary>
        /// The function called when the user wants to sign in with Passport but from
        /// the username/password dialog.
        ///
        /// First we check if they already use Passport as their primary login method
        ///
        /// Otherwise we authenticate their username/password and then begin the Passport
        /// enrollment process which consists of creating a Passport key and then sending that to
        /// the authentication server.
        /// </summary>
        /// <param name="passportIsPrimaryLogin">Boolean representing if primary login method is Passport</param>
        private async void SignInPassport(bool passportIsPrimaryLogin)
        {
            if (passportIsPrimaryLogin == true)
            {
                if (await AuthenticatePassport() == true)
                {
                    SuccessfulSignIn(m_account);
                    return;
                }
            }
            else if (await SignInPassword(true) == true)
            {
                if (await CreatePassportKey(textbox_Username.Text) == true)
                {
                    bool serverAddedPassportToAccount = await AddPassportToAccountOnServer();

                    if (serverAddedPassportToAccount == true)
                    {
                        rootPage.NotifyUser("Successfully signed in with Microsoft Passport!", NotifyType.StatusMessage);
                        if (m_addingAccount == true)
                        {
                            Account goodAccount = new Account() { Name = textbox_Username.Text, Email = textbox_Username.Text, UsesPassport = true };
                            SuccessfulSignIn(goodAccount);
                        }
                        else
                        {
                            m_account.UsesPassport = true;
                            SuccessfulSignIn(m_account);
                        }
                        return;
                    }
                }
                textblock_ErrorField.Text = "Sign In with Passport failed. Try later.";
                button_PassportSignIn.IsEnabled = false;
            }
            textblock_ErrorField.Text = "Invalid username or password.";
        }

        /// <summary>
        /// Method called when a user clicks on the username/password Sign In button.
        ///
        /// Will attempt to authenticate the credentials with the server and then either sign in
        /// or display an error to the user if applicable.
        /// </summary>
        /// <param name="calledFromPassport">Boolean representing if this method was called from the user clicking "Sign In with Microsoft Passport"</param>
        /// <returns>Boolean representing if signing in with password succeeded</returns>
        private async Task<bool> SignInPassword(bool calledFromPassport)
        {
            textblock_ErrorField.Text = "";

            if (textbox_Username.Text.Length == 0 || passwordbox_Password.Password.Length == 0)
            {
                textblock_ErrorField.Text = "Username/Password cannot be blank.";
                return false;
            }

            try
            {
                bool signedIn = await AuthenticatePasswordCredentials();

                if (signedIn == false)
                {
                    textblock_ErrorField.Text = "Unable to sign you in with those credentials.";
                }
                else
                {
                    // TODO: Roaming Passport settings. Make it so the server can tell us if they prefer to use Passport and upsell immediately.

                    Account goodAccount = new Account() { Name = textbox_Username.Text, Email = textbox_Username.Text, UsesPassport = false };
                    if (calledFromPassport == false)
                    {
                        rootPage.NotifyUser("Successfully signed in with traditional username/password!", NotifyType.StatusMessage);
                        SuccessfulSignIn(goodAccount);
                    }

                    return true;
                }

                return false;
            }
            catch (Exception e)
            {
                rootPage.NotifyUser(e.Message, NotifyType.ErrorMessage);
                return false;
            }
        }

        /// <summary>
        /// Authenticate user credentials with server and return result.
        /// </summary>
        /// <returns>Boolean representing if authenticating the user credentials with the server succeeded</returns>
        private async Task<bool> AuthenticatePasswordCredentials()
        {
            // TODO: Authenticate with server once that part is done for the sample.

            return true;
        }

        /// <summary>
        /// Attempts to get the authentication message from the Passport key for this account.
        ///
        /// This will bring up the Passport PIN dialog and prompt the user for their PIN.
        /// 
        /// The authentication message will be null if signing fails.
        /// </summary>
        /// <returns>Boolean representing if authenticating Passport succeeded</returns>
        private async Task<bool> AuthenticatePassport()
        {
            IBuffer message = CryptographicBuffer.ConvertStringToBinary("LoginAuth", BinaryStringEncoding.Utf8);
            IBuffer authMessage = await GetPassportAuthenticationMessage(message, m_account.Email);

            if (authMessage != null)
            {
                return true;
            }

            return false;
        }

        /// <summary>
        /// Handles user saving for our list of users if this is a new user
        /// and navigates to the main content.
        /// </summary>
        /// <param name="account">The account used to sign in</param>
        private void SuccessfulSignIn(Account account)
        {
            // If this is an already existing account, replace the old
            // version of this account in the account list.
            if (m_addingAccount == false)
            {
                foreach (Account a in UserSelect.accountList)
                {
                    if (a.Email == account.Email)
                    {
                        UserSelect.accountList.Remove(a);
                        break;
                    }
                }
            }

            UserSelect.accountList.Add(account);

            CleanUpUserList();
            this.Frame.Navigate(typeof(Content), account);
        }

        /// <summary>
        /// Function to be called when we need to register our public key with
        /// the server for Microsoft Passport
        /// </summary>
        /// <returns>Boolean representing if adding the Passport login method to this account on the server succeeded</returns>
        private async Task<bool> AddPassportToAccountOnServer()
        {
            // TODO: Add Passport signing info to server when that part is done for the sample

            return true;
        }

        /// <summary>
        /// Uses the AccountsHelper class to save the account list before we move on from
        /// the sign in process
        /// </summary>
        private void CleanUpUserList()
        {
            AccountsHelper.SaveAccountList(UserSelect.accountList);
        }

        /// <summary>
        /// Creates a Passport key on the machine using the account id passed.
        /// Then returns a boolean based on whether we were able to create a Passport key or not.
        ///
        /// Will also attempt to create an attestation that this key is backed by hardware on the device, but is not a requirement
        /// for a working key in this scenario. It is possible to not accept a key that is software-based only.
        /// </summary>
        /// <param name="accountId">The account id associated with the account that we are enrolling into Passport</param>
        /// <returns>Boolean representing if creating the Passport key succeeded</returns>
        private async Task<bool> CreatePassportKey(string accountId)
        {
            KeyCredentialRetrievalResult keyCreationResult = await KeyCredentialManager.RequestCreateAsync(accountId, KeyCredentialCreationOption.ReplaceExisting);

            if (keyCreationResult.Status == KeyCredentialStatus.Success)
            {
                rootPage.NotifyUser("Successfully made key", NotifyType.StatusMessage);

                KeyCredential userKey = keyCreationResult.Credential;
                IBuffer publicKey = userKey.RetrievePublicKey();
                KeyCredentialAttestationResult keyAttestationResult = await userKey.GetAttestationAsync();

                IBuffer keyAttestation = null;
                IBuffer certificateChain = null;
                bool keyAttestationIncluded = false;
                bool keyAttestationCanBeRetrievedLater = false;
                KeyCredentialAttestationStatus keyAttestationRetryType = 0;

                if (keyAttestationResult.Status == KeyCredentialAttestationStatus.Success)
                {
                    keyAttestationIncluded = true;
                    keyAttestation = keyAttestationResult.AttestationBuffer;
                    certificateChain = keyAttestationResult.CertificateChainBuffer;
                    rootPage.NotifyUser("Successfully made key and attestation", NotifyType.StatusMessage);
                }
                else if (keyAttestationResult.Status == KeyCredentialAttestationStatus.TemporaryFailure)
                {
                    keyAttestationRetryType = KeyCredentialAttestationStatus.TemporaryFailure;
                    keyAttestationCanBeRetrievedLater = true;
                    rootPage.NotifyUser("Successfully made key but not attestation", NotifyType.StatusMessage);
                }
                else if (keyAttestationResult.Status == KeyCredentialAttestationStatus.NotSupported)
                {
                    keyAttestationRetryType = KeyCredentialAttestationStatus.NotSupported;
                    keyAttestationCanBeRetrievedLater = false;
                    rootPage.NotifyUser("Key created, but key attestation not supported", NotifyType.StatusMessage);
                }

                // Package public key, keyAttesation if available, 
                // certificate chain for attestation endorsement key if available,  
                // status code of key attestation result: keyAttestationIncluded or 
                // keyAttestationCanBeRetrievedLater and keyAttestationRetryType
                // and send it to application server to register the user.
                bool serverAddedPassportToAccount = await AddPassportToAccountOnServer();

                if (serverAddedPassportToAccount == true)
                {
                    return true;
                }
            }
            else if (keyCreationResult.Status == KeyCredentialStatus.UserCanceled)
            {
                // User cancelled the Passport enrollment process
            }
            else if (keyCreationResult.Status == KeyCredentialStatus.NotFound)
            {
                // User needs to create PIN
                textblock_PassportStatusText.Text = "Microsoft Passport is almost ready!\nPlease go to Windows Settings and set up a PIN to use it.";
                grid_PassportStatus.Background = new SolidColorBrush(Color.FromArgb(255, 50, 170, 207));
                button_PassportSignIn.IsEnabled = false;

                m_passportAvailable = false;
            }
            else
            {
                rootPage.NotifyUser(keyCreationResult.Status.ToString(), NotifyType.ErrorMessage);
            }

            return false;
        }

        /// <summary>
        /// Attempts to sign a message using the Passport key on the system for the accountId passed.
        /// </summary>
        /// <param name="message">The message to be signed</param>
        /// <param name="accountId">The account id for the Passport key we are using to sign</param>
        /// <returns>Boolean representing if creating the Passport authentication message succeeded</returns>
        private async Task<IBuffer> GetPassportAuthenticationMessage(IBuffer message, string accountId)
        {
            KeyCredentialRetrievalResult openKeyResult = await KeyCredentialManager.OpenAsync(accountId);

            if (openKeyResult.Status == KeyCredentialStatus.Success)
            {
                KeyCredential userKey = openKeyResult.Credential;
                IBuffer publicKey = userKey.RetrievePublicKey();

                KeyCredentialOperationResult signResult = await userKey.RequestSignAsync(message);

                if (signResult.Status == KeyCredentialStatus.Success)
                {
                    return signResult.Result;
                }
                else if (signResult.Status == KeyCredentialStatus.UserCanceled)
                {
                    // User cancelled the Passport PIN entry.
                    //
                    // We will return null below this and the username/password
                    // sign in form will show.
                }
                else if (signResult.Status == KeyCredentialStatus.NotFound)
                {
                    // Must recreate Passport key
                }
                else if (signResult.Status == KeyCredentialStatus.SecurityDeviceFailure || signResult.Status == KeyCredentialStatus.SecurityDeviceLocked)
                {
                    // Can't use Passport right now, remember that hardware failed and suggest restart
                }
                else if (signResult.Status == KeyCredentialStatus.UnknownError)
                {
                    // Can't use Passport right now, try again later
                }

                return null;
            }
            else if (openKeyResult.Status == KeyCredentialStatus.NotFound)
            {
                // Passport key lost, need to recreate it
                textblock_PassportStatusText.Text = "Microsoft Passport is almost ready!\nPlease go to Windows Settings and set up a PIN to use it.";
                grid_PassportStatus.Background = new SolidColorBrush(Color.FromArgb(255, 50, 170, 207));
                button_PassportSignIn.IsEnabled = false;

                m_passportAvailable = false;
            }
            else
            {
                // Can't use Passport right now, try again later
            }
            return null;
        }
    }
}