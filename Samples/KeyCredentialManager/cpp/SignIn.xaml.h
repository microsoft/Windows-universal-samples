#pragma once
#include "SignIn.g.h"
#include "MainPage.xaml.h"
#include "Account.h"
#include "pch.h"
#include "SignIn.xaml.h"
#include "MainPage.xaml.h"
#include "UserSelect.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// TODO
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class SignIn sealed
    {
    public:
        SignIn();

    protected:
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void Button_SignIn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void Button_Passport_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void PassportAvailableCheck();
        void SignInPassport(bool passportIsPrimaryLogin);
        bool SignInPassword(bool calledFromPassport);
        bool AuthenticatePasswordCredentials();
        bool AuthenticatePassport();
        bool AddPassportToAccountOnServer();
        void SuccessfulSignIn(Account^ account);
        bool CreatePassportKey(Platform::String^ accountId);
        Windows::Storage::Streams::IBuffer^ GetPassportAuthenticationMessage(Windows::Storage::Streams::IBuffer^ message, Platform::String^ accountId);

        MainPage^ rootPage;
        bool m_addingAccount = true;
        Account^ m_account;
        bool m_passportAvailable = true;
    };
}