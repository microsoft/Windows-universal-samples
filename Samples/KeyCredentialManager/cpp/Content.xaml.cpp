#include "pch.h"
#include "Content.xaml.h"
#include "UserSelect.xaml.h"
#include "MainPage.xaml.h"

using namespace SDKTemplate;
using namespace Platform;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::Security::Credentials;

Content::Content() 
{
    InitializeComponent();
    rootPage = MainPage::Current;
}

void Content::OnNavigatedTo(NavigationEventArgs^ e)
{
    m_activeAccount = (Account^) e->Parameter;
}

void Content::Button_Restart_Click(Object^ sender, RoutedEventArgs^ e)
{
    TypeName userSelectType = { "SDKTemplate.UserSelect", TypeKind::Custom };
    this->Frame->Navigate(userSelectType);
}

void Content::Button_Forget_Click(Object^ sender, RoutedEventArgs^ e)
{
    PassportDelete();

    //Remove account from the in-memory account list
    for (unsigned int i = 0; i < UserSelect::m_accountList->Size; i++)
    {
        Account^ a = UserSelect::m_accountList->GetAt(i);
        if (a->Email == m_activeAccount->Email)
        {
            UserSelect::m_accountList->RemoveAt(i);
            break;
        }
    }
}

/// <summary>
/// Checks the KeyCredentialManager to see if there is a Passport for the current user and
/// sends the informaton to the server to unregister it.
/// 
/// Then deletes the local key associated with the Passport.
/// </summary>
void Content::PassportDelete()
{
    if (m_activeAccount->UsesPassport == true)
    {
        concurrency::create_task(KeyCredentialManager::OpenAsync(m_activeAccount->Email))
            .then([this](KeyCredentialRetrievalResult^ keyOpenResult)
        {
            if (keyOpenResult->Status == KeyCredentialStatus::Success)
            {
                auto userKey = keyOpenResult->Credential;
                auto publicKey = userKey->RetrievePublicKey();

                // Send key information to server to unregister it
                DeletePassportServerSide();

                concurrency::create_task(KeyCredentialManager::DeleteAsync(m_activeAccount->Email))
                    .get();
            }
        });
    }

    rootPage->NotifyUser("User " + m_activeAccount->Email + " deleted.", NotifyType::StatusMessage);
    button_Forget->IsEnabled = false;
    button_Forget->Content = "User Forgotten.";
}

/// <summary>
/// Function used to unregister user's Passport on the server side
/// </summary>
/// <returns></returns>
bool Content::DeletePassportServerSide()
{
    return true;
}