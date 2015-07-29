#include "pch.h"
#include "SignIn.xaml.h"

using namespace Windows::Security::Credentials;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml;
using namespace Windows::UI;
using namespace Windows::Storage::Streams;
using namespace Windows::Security::Cryptography;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Navigation;

SDKTemplate::SignIn::SignIn()
{
    InitializeComponent();
}

void SDKTemplate::SignIn::OnNavigatedTo(NavigationEventArgs ^ e)
{
    rootPage = MainPage::Current;
    PassportAvailableCheck();

    if (e->Parameter != nullptr)
    {
        m_account = (Account^) e->Parameter;
        textbox_Username->Text = m_account->Email;
        textbox_Username->IsEnabled = false;
        m_addingAccount = false;

        if (m_account->UsesPassport == true)
        {
            SignInPassport(true);
        }
    }
}

void SDKTemplate::SignIn::Button_SignIn_Click(Object ^ sender, RoutedEventArgs ^ e)
{
    SignInPassword(false);
}

void SDKTemplate::SignIn::Button_Passport_Click(Object ^ sender, RoutedEventArgs ^ e)
{
    if (m_addingAccount == false)
    {
        SignInPassport(m_account->UsesPassport);
    }
    else
    {
        SignInPassport(false);
    }
}

void SDKTemplate::SignIn::PassportAvailableCheck()
{
    concurrency::create_task(KeyCredentialManager::IsSupportedAsync())
        .then([this](bool keyCredentialAvailable) 
    {
        if (keyCredentialAvailable == false)
        {
            //
            // Key credential is not enabled yet as user 
            // needs to connect to a MSA account and select a pin in the connecting flow.
            //
            textblock_PassportStatusText->Text = "Microsoft Passport is not set up.\nPlease go to Windows Settings and connect an MSA account or set up a PIN!";
            button_PassportSignIn->IsEnabled = false;
            m_passportAvailable = false;
            grid_PassportStatus->Background = ref new SolidColorBrush(ColorHelper::FromArgb(255,50,170,207));
        }
    });
}

void SDKTemplate::SignIn::SignInPassport(bool passportIsPrimaryLogin)
{
    if (passportIsPrimaryLogin == true)
    {
        if (AuthenticatePassport() == true)
        {
            SuccessfulSignIn(m_account);
            return;
        }
    }
    else if (SignInPassword(true) == true)
    {
        if (CreatePassportKey(textbox_Username->Text) == true)
        {
            bool serverAddedPassportToAccount = AddPassportToAccountOnServer();

            if (serverAddedPassportToAccount == true)
            {
                rootPage->NotifyUser("Successfully signed in with Microsoft Passport!", NotifyType::StatusMessage);
                if (m_addingAccount == true)
                {
                    Account^ goodAccount = ref new Account();
                    goodAccount->Name = textbox_Username->Text;
                    goodAccount->Email = textbox_Username->Text;
                    goodAccount->UsesPassport = true;
                    SuccessfulSignIn(goodAccount);
                }
                else
                {
                    m_account->UsesPassport = true;
                    SuccessfulSignIn(m_account);
                }
                return;
            }
        }
    }
    textblock_ErrorField->Text = "Sign In with Passport failed. Try later.";
    button_PassportSignIn->IsEnabled = false;
}

bool SDKTemplate::SignIn::SignInPassword(bool calledFromPassport)
{
    textblock_ErrorField->Text = "";

    if (textbox_Username->Text->IsEmpty() || passwordbox_Password->Password->IsEmpty())
    {
        textblock_ErrorField->Text = "Username/Password cannot be blank.";
        return false;
    }

    try
    {
        bool signedIn = AuthenticatePasswordCredentials();

        if (signedIn == false)
        {
            textblock_ErrorField->Text = "Unable to sign you in with those credentials.";
        }
        else
        {
            // Developer TODO: Roaming Passport settings. Make it so the server can tell if they prefer to use Passport and upsell immediately.

            Account^ goodAccount = ref new Account();
            goodAccount->Name = textbox_Username->Text;
            goodAccount->Email = textbox_Username->Text;
            goodAccount->UsesPassport = true;
            if (calledFromPassport == false)
            {
                rootPage->NotifyUser("Successfully signed in with traditional username/password!", NotifyType::StatusMessage);
                goodAccount->UsesPassport = false;
                SuccessfulSignIn(goodAccount);
            }

            return true;
        }

        return false;
    }
    catch (Platform::Exception^ e)
    {
        rootPage->NotifyUser(e->Message, NotifyType::ErrorMessage);
        return false;
    }
}

bool SDKTemplate::SignIn::AuthenticatePasswordCredentials()
{
    // Developer TODO: Authenticate with server
    return true;
}

bool SDKTemplate::SignIn::AuthenticatePassport()
{
    IBuffer^ message = CryptographicBuffer::ConvertStringToBinary("LoginAuth", BinaryStringEncoding::Utf8);
    IBuffer^ authMessage = GetPassportAuthenticationMessage(message, m_account->Email);

    if (authMessage != nullptr)
    {
        return true;
    }

    return false;
}

void SDKTemplate::SignIn::SuccessfulSignIn(Account ^ account)
{
    // If this is an already existing account, replace the old
    // version of this account in the account list.
    if (m_addingAccount == false)
    {
        for (unsigned int i = 0; i < UserSelect::m_accountList->Size; i++)
        {
            Account^ a = UserSelect::m_accountList->GetAt(i);
            if (a->Email == account->Email)
            {
                UserSelect::m_accountList->GetAt(i)->UsesPassport = a->UsesPassport;
                break;
            }
        }
    }
    else
    {
        UserSelect::m_accountList->Append(account);
    }

    TypeName contentType = { "SDKTemplate.Content", TypeKind::Custom };
    this->Frame->Navigate(contentType, account);
}

bool SDKTemplate::SignIn::AddPassportToAccountOnServer()
{
    // Developer TODO: Add Passport signing info to server
    return true;
}

bool SDKTemplate::SignIn::CreatePassportKey(Platform::String^ accountId)
{
    concurrency::create_task(KeyCredentialManager::RequestCreateAsync(accountId, KeyCredentialCreationOption::ReplaceExisting))
        .then([this](KeyCredentialRetrievalResult^ keyCreationResult)
    {
        if (keyCreationResult->Status == KeyCredentialStatus::Success)
        {
            rootPage->NotifyUser("Successfully made key", NotifyType::StatusMessage);

            KeyCredential^ userKey = keyCreationResult->Credential;
            concurrency::create_task(userKey->GetAttestationAsync())
                .then([this](KeyCredentialAttestationResult^ keyAttestationResult) 
            {
                IBuffer^ keyAttestation = nullptr;
                IBuffer^ certificateChain = nullptr;
                bool keyAttestationIncluded = false;
                bool keyAttestationCanBeRetrievedLater = false;
                KeyCredentialAttestationStatus keyAttestationRetryType = KeyCredentialAttestationStatus::Success;

                if (keyAttestationResult->Status == KeyCredentialAttestationStatus::Success)
                {
                    keyAttestationIncluded = true;
                    keyAttestation = keyAttestationResult->AttestationBuffer;
                    certificateChain = keyAttestationResult->CertificateChainBuffer;
                    rootPage->NotifyUser("Successfully made key and attestation", NotifyType::StatusMessage);
                }
                else if (keyAttestationResult->Status == KeyCredentialAttestationStatus::TemporaryFailure)
                {
                    keyAttestationRetryType = KeyCredentialAttestationStatus::TemporaryFailure;
                    keyAttestationCanBeRetrievedLater = true;
                    rootPage->NotifyUser("Successfully made key but not attestation", NotifyType::StatusMessage);
                }
                else if (keyAttestationResult->Status == KeyCredentialAttestationStatus::NotSupported)
                {
                    keyAttestationRetryType = KeyCredentialAttestationStatus::NotSupported;
                    keyAttestationCanBeRetrievedLater = false;
                    rootPage->NotifyUser("Key created, but key attestation not supported", NotifyType::StatusMessage);
                }

                // Package public key, keyAttesation if available, 
                // certificate chain for attestation endorsement key if available,  
                // status code of key attestation result: keyAttestationIncluded or 
                // keyAttestationCanBeRetrievedLater and keyAttestationRetryType
                // and send it to application server to register the user.
                bool serverAddedPassportToAccount = AddPassportToAccountOnServer();

                if (serverAddedPassportToAccount == true)
                {
                    return true;
                }
            }); 
        }
        else if (keyCreationResult->Status == KeyCredentialStatus::UserCanceled)
        {
            // User cancelled the Passport enrollment process
        }
        else if (keyCreationResult->Status == KeyCredentialStatus::NotFound)
        {
            // User needs to create PIN
            textblock_PassportStatusText->Text = "Microsoft Passport is almost ready!\nPlease go to Windows Settings and set up/reset your PIN to use it.";

            grid_PassportStatus->Background = ref new SolidColorBrush(ColorHelper::FromArgb(255, 50, 170, 207));
            button_PassportSignIn->IsEnabled = false;

            m_passportAvailable = false;
        }
        else
        {
            rootPage->NotifyUser(keyCreationResult->Status.ToString(), NotifyType::ErrorMessage);
        }
    }).wait();

    return false;
}

Windows::Storage::Streams::IBuffer ^ SDKTemplate::SignIn::GetPassportAuthenticationMessage(IBuffer ^ message, Platform::String ^ accountId)
{
    concurrency::create_task(KeyCredentialManager::OpenAsync(accountId))
        .then([this, message](KeyCredentialRetrievalResult^ openKeyResult) 
    {
        if (openKeyResult->Status == KeyCredentialStatus::Success)
        {
            KeyCredential^ userKey = openKeyResult->Credential;

            concurrency::create_task(userKey->RequestSignAsync(message))
                .then([this, message](KeyCredentialOperationResult^ signResult)
            {
                if (signResult->Status == KeyCredentialStatus::Success)
                {
                    return signResult->Result;
                }
                else if (signResult->Status == KeyCredentialStatus::UserCanceled)
                {
                    // User cancelled the Passport PIN entry.
                    //
                    // We will return null below this and the username/password
                    // sign in form will show.
                }
                else if (signResult->Status == KeyCredentialStatus::NotFound)
                {
                    // Must recreate Passport key
                }
                else if (signResult->Status == KeyCredentialStatus::SecurityDeviceFailure || signResult->Status == KeyCredentialStatus::SecurityDeviceLocked)
                {
                    // Can't use Passport right now, remember that hardware failed and suggest restart
                }
                else if (signResult->Status == KeyCredentialStatus::UnknownError)
                {
                    // Can't use Passport right now, try again later
                }
            }).wait();
        }
        else if (openKeyResult->Status == KeyCredentialStatus::NotFound)
        {
            // Passport key lost, need to recreate it
            m_passportAvailable = false;
        }
        else
        {
            // Can't use Passport right now, try again later
        }
    });

    return nullptr;
}
